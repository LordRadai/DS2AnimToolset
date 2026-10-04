"""
xml2mcn.py - rebuild a morphemeConnect 3.6 network (.mcn) from a compiled network export XML.

The export XML (NetworkDefinition/Node/DataBlock) is what Connect's per-node Lua serialize()
functions write.  This tool inverts those serializers and emits a Lua script that recreates
the network through Connect's own scripting API (create/connect/setAttribute/...).  Connect
then writes the .mcn itself, so manifest versions, default attributes and pins are correct.

Usage:
  python xml2mcn.py <network.xml> [--library X_Library.xml] [--rig X.mrarig] [--model X.xmd]
                    [--out-dir DIR]

Workflow (create() cannot make ActiveState nodes, hence two Connect passes):
  1. python xml2mcn.py X.xml                  -> X.mcp, X_rebuild.lua, X_stage2.lua
  2. morphemeConnect.exe -nogui -script X_rebuild.lua
  3. python xml2mcn.py X.xml --inject         -> adds ActiveState nodes to X.mcn
  4. morphemeConnect.exe -nogui -script X_stage2.lua
  5. python xmldiff.py X.xml roundtrip\\X.xml

Control parameters: tools\\ds2_control_parameters.json (or --cp-config) sets min/max/default by CP
name; its default wins over the export's.  --cp-template adds a network's CPs to that file,
--cp-only writes <name>_cparams.lua to apply it to an existing .mcn without rebuilding.
The "group" field puts the CP in that ControlParameterGroup; there is no Lua API for CP groups,
so --inject (and --cp-only) write the groups straight into the .mcn.

Outputs (in out-dir, default = folder of the XML):
  <name>.mcp              project file (Z-up, metres) so $(RootDir) resolves to out-dir
  <name>_rebuild.lua      run with: morphemeConnect.exe -nogui -script <name>_rebuild.lua
  <name>_rebuild.log      (written by the script) every API call and its result
  <name>.mcn              (written by the script) the reconstructed network
  roundtrip\\<name>.xml    (written by the script) Connect's re-export, for xmldiff.py
"""
import argparse, math, os, sys, xml.etree.ElementTree as ET
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mcnxml

# runtime node type ids -> Connect manifest type names
CP_TYPES = {20: 'Float', 21: 'Vector3', 22: 'Vector4', 23: 'Bool', 24: 'Int', 25: 'UInt'}
NODE_TYPES = {104: 'AnimWithEvents', 122: 'HeadLook'}
SM_TYPE, TRANSIT_TYPES, NETWORK_TYPE = 10, {402: 'Transit'}, 9
DEFAULT_CP_CONFIG = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'ds2_control_parameters.json')
COND_TYPES = {601: 'MessageCondition', 603: 'FractionThroughSource', 609: 'ControlParamInRange',
              611: 'InEventRange', 604: 'AtEvent', 605: 'DuringEvent', 616: 'ControlParamTest',
              617: 'InSubState', 618: 'InDurationEvent', 602: 'UserDataEvent', 610: 'FractionThroughDurationEvent'}

# Blend tree input pins: export field name -> Connect pin name (node data that references another node)
INPUT_PINS = {
    'AnimWithEvents': {},
    'HeadLook': {'InputNodeID': 'Source', 'Target': 'Target', 'BlendWeight': 'BlendWeight'},
}


def lua_str(s):
    return '"' + s.replace('\\', '\\\\').replace('"', '\\"').replace('\n', '\\n') + '"'


def lua_val(v):
    if isinstance(v, bool): return 'true' if v else 'false'
    if isinstance(v, int): return str(v)
    if isinstance(v, float):
        if math.isnan(v) or math.isinf(v): return '0'
        return repr(float('%.9g' % v))
    if isinstance(v, str): return lua_str(v)
    if isinstance(v, dict): return '{ ' + ', '.join('%s = %s' % (k, lua_val(x)) for k, x in v.items()) + ' }'
    if isinstance(v, (list, tuple)): return '{ ' + ', '.join(lua_val(x) for x in v) + ' }'
    raise TypeError(v)


class Library:
    def __init__(self, path):
        self.sets = []  # [(name, {index: entry})]
        if not path or not os.path.exists(path): return
        r = ET.parse(path).getroot()
        for s in r.findall('AnimationSet'):
            entries = {}
            for e in s.findall('AnimationEntry'):
                entries[int(e.get('index'))] = dict(
                    filename=e.findtext('animFile'), takename=e.findtext('take'),
                    synctrack=e.findtext('syncTrack') or '', format=e.get('format') or 'nsa',
                    options=e.get('options') or '')
            self.sets.append((s.get('name'), entries))


def load_rig_joints(path):
    if not path or not os.path.exists(path): return {}
    r = ET.parse(path).getroot()
    return {int(j.get('index')): j.get('name') for j in r.iter('Joint')}, r


class Converter:
    def __init__(self, xml_path, lib, joints, rig_root, out_dir, model, cp_config=None):
        self.cp_config, self.cp_used = cp_config or {}, set()
        self.root, self.nodes, self.msgs = mcnxml.load(xml_path)
        self.name = self.root.get('name') or os.path.splitext(os.path.basename(xml_path))[0]
        self.lib, self.joints, self.rig_root, self.out_dir, self.model = lib, joints, rig_root, out_dir, model
        self.lines = []
        self.unsupported = []

    # ------------------------------------------------------------------ helpers
    def emit(self, s=''): self.lines.append(s)

    def call(self, desc, expr, var=None):
        """Emit a pcall-wrapped API call; result stored in var (Lua local table P[...] for paths)."""
        tgt = (var + ' = ') if var else ''
        self.emit('%sTRY(%s, function() return %s end)' % (tgt, lua_str(desc), expr))

    def short(self, n): return n.name.split('|')[-1]

    def path_of(self, comps): return '|'.join(comps)

    # Graph structure is recovered from the '|' separated export names:
    #   SM|State|Node  -> State is a BlendTree state (not itself in the XML) when SM is a state machine.
    def analyse(self):
        nodes = self.nodes
        self.byname = {n.name: n for n in nodes.values()}
        self.sm_ids = [n.id for n in nodes.values() if n.type == SM_TYPE]
        self.graph_nodes = [n for n in nodes.values() if n.type in NODE_TYPES]
        self.transits = [n for n in nodes.values() if n.type in TRANSIT_TYPES]
        self.cps = [n for n in nodes.values() if n.type in CP_TYPES]
        others = [n for n in nodes.values() if n.type not in NODE_TYPES and n.type not in TRANSIT_TYPES
                  and n.type not in CP_TYPES and n.type not in (SM_TYPE, NETWORK_TYPE)]
        for n in others: self.unsupported.append('node #%d %s (typeID %d)' % (n.id, n.name, n.type))

        # every BlendTree state container: a path prefix whose parent component is a state machine
        sm_names = {nodes[i].name for i in self.sm_ids}
        self.bt_states = set()
        for n in self.graph_nodes:
            comps = n.name.split('|')
            for k in range(1, len(comps)):
                parent, here = '|'.join(comps[:k]), '|'.join(comps[:k + 1])
                if parent in sm_names and here not in self.byname:
                    self.bt_states.add(here)
        # state (direct child path of an SM) for a runtime node id
        self.state_of = {}
        for n in nodes.values():
            if n.type in (SM_TYPE,) or n.type in NODE_TYPES:
                comps = n.name.split('|')
                for k in range(len(comps) - 1, 0, -1):
                    if '|'.join(comps[:k]) in sm_names:
                        self.state_of[n.id] = '|'.join(comps[:k + 1]); break
        self.root_id = int(self.root.findtext('rootNodeNetworkID'))

    # ------------------------------------------------------------------ script sections
    def prologue(self, log_suffix='_rebuild.log'):
        out = self.out_dir
        self.emit('-- Generated by xml2mcn.py from %s. Run: morphemeConnect.exe -nogui -script <this file>' % self.name)
        self.emit('local ROOT = %s' % lua_str(out))
        self.emit('local NAME = %s' % lua_str(self.name))
        self.emit('local LOGF = io.open(ROOT .. "\\\\" .. NAME .. "%s", "w")' % log_suffix)
        self.emit('local NFAIL = 0')
        self.emit('local function LOG(s) LOGF:write(s .. "\\n"); LOGF:flush() end')
        self.emit('local function TS(v) if type(v) == "table" then local t = {} for k, x in pairs(v) do table.insert(t, tostring(k) .. "=" .. tostring(x)) end return "{" .. table.concat(t, ", ") .. "}" end return tostring(v) end')
        self.emit('function TRY(desc, f)')
        self.emit('  local ok, r1, r2 = pcall(f)')
        self.emit('  if not ok then NFAIL = NFAIL + 1; LOG("FAIL  " .. desc .. "  :: " .. tostring(r1)); return nil end')
        self.emit('  if r1 == nil or r1 == false then NFAIL = NFAIL + 1; LOG("NIL   " .. desc .. "  -> " .. TS(r1) .. " " .. TS(r2)); return r1 end')
        self.emit('  LOG("OK    " .. desc .. "  -> " .. TS(r1)); return r1')
        self.emit('end')
        self.emit('-- try several alternatives until one returns a truthy value')
        self.emit('function TRYANY(desc, fs)')
        self.emit('  for i, f in ipairs(fs) do local ok, r = pcall(f); if ok and r then LOG("OK    " .. desc .. " (variant " .. i .. ") -> " .. TS(r)); return r end end')
        self.emit('  NFAIL = NFAIL + 1; LOG("FAIL  " .. desc .. " (all variants)"); return nil')
        self.emit('end')
        self.emit('local P = {}   -- export name -> Connect path')
        self.emit('LOG("commandLine=" .. tostring(mcn.inCommandLineMode()))')
        self.emit()

    def setup(self):
        set_name = self.lib.sets[0][0] if self.lib.sets else 'AnimSet'
        self.set_name = set_name
        rig_rel = '%s.mcarig' % set_name
        hip = traj = None
        if self.rig_root is not None:
            hip = self.joints.get(int(self.rig_root.findtext('hipIndex') or -1))
            traj = self.joints.get(int(self.rig_root.findtext('trajectoryIndex') or -1))
        self.emit('-- 1. animation rig + project')
        self.emit('local RIG = ROOT .. %s' % lua_str('\\' + rig_rel))
        self.emit('local fh = io.open(RIG, "r")')
        self.emit('if fh then fh:close(); LOG("rig exists " .. RIG) else')
        if self.model:
            self.emit('  TRY("anim.createRig", function() return anim.createRig(ROOT .. %s, RIG, %s, %s) end)' % (
                lua_str('\\' + os.path.basename(self.model)), lua_val(hip or ''), lua_val(traj or '')))
        self.emit('end')
        self.emit('TRY("project.open", function() return project.open(ROOT .. "\\\\" .. NAME .. ".mcp") end)')
        self.emit('TRY("mcn.new", function() return mcn.new() end)')
        self.emit('LOG("project=" .. TS(project.getFilename()) .. " root=" .. TS(mcn.getProjectRoot()))')
        self.emit('local SET = %s' % lua_str(set_name))
        self.emit('local sets = listAnimSets() or {}')
        self.emit('LOG("animsets=" .. TS(sets))')
        self.emit('local haveSet = false')
        self.emit('for i, s in ipairs(sets) do if s == SET then haveSet = true end end')
        self.emit('if not haveSet then')
        self.emit('  if table.getn(sets) == 1 then TRY("rename animset", function() return rename(sets[1], SET) end) end')
        self.emit('  sets = listAnimSets() or {}; for i, s in ipairs(sets) do if s == SET then haveSet = true end end')
        self.emit('  if not haveSet then TRY("createAnimSet", function() return createAnimSet(SET, "nsa", "", "$(RootDir)\\\\%s") end) end' % rig_rel)
        self.emit('end')
        self.emit('TRY("setSelectedAnimSet", function() return setSelectedAnimSet(SET) end)')
        self.emit('TRY("anim.getRigPath", function() return anim.getRigPath(SET) end)')
        self.emit('if not anim.getRigPath(SET) or anim.getRigPath(SET) == "" then TRY("anim.setRigPath", function() return anim.setRigPath("$(RootDir)\\\\%s", SET, true) end) end' % rig_rel)
        self.emit('LOG("root children=" .. TS(listChildren("")))')
        self.emit()

    def requests(self):
        self.emit('-- 2. requests (runtime ids preserved)')
        self.req_var = {}
        for m in sorted(self.msgs, key=lambda m: int(m['messageID'])):
            if m.get('messageTypeID') != '10':
                self.unsupported.append('message %s type %s' % (m['name'], m.get('messageTypeID'))); continue
            mid = int(m['messageID'])
            self.call('createRequest %s' % m['name'], 'createRequest(%s, %d)' % (lua_str(m['name']), mid), 'P[%s]' % lua_str('#msg%d' % mid))
            self.req_var[mid] = 'P[%s]' % lua_str('#msg%d' % mid)
        self.emit()

    def control_params(self):
        self.emit('-- 3. control parameters')
        for n in sorted(self.cps, key=lambda n: n.id):
            dt = CP_TYPES[n.type]
            nm = self.short(n)
            key = 'P[%s]' % lua_str(n.name)
            variants = [dt, dt.lower()]
            self.emit('%s = TRYANY(%s, { %s })' % (key, lua_str('createControlParameter %s %s' % (dt, nm)),
                      ', '.join('function() return createControlParameter(%s, %s) end' % (lua_str(v), lua_str(nm)) for v in variants)))
            self.cp_settings(n, key)
        self.emit()

    def xml_default(self, n):
        if n.type in (21, 22):
            return [n.get('DefaultValue_%d' % i, 0.0) for i in range(3 if n.type == 21 else 4)]
        if n.type in (24, 25):
            return [n.get('DefaultInt', n.get('DefaultUInt', 0))]
        if n.type == 23:
            return [1.0 if n.get('DefaultBool', False) else 0.0]
        return [n.get('DefaultValue_0', 0.0)]

    def cp_settings(self, n, key, force=False):
        """Range and default for one CP. The config (by CP name) wins over the export's default."""
        nm = self.short(n)
        cfg = self.cp_config.get(nm, {})
        rng = {k: cfg[k] for k in ('min', 'max') if cfg.get(k) is not None}
        if rng:
            self.call('setRange %s' % nm, 'setRange(%s, %s)' % (key, lua_val({k: float(v) for k, v in rng.items()})))
        vals = cfg.get('default')
        if vals is None:
            vals = self.xml_default(n)
        elif not isinstance(vals, list):
            vals = [vals]
        if force or 'default' in cfg or any(v != 0 for v in vals):
            self.call('setDefaultValue %s' % nm, 'setDefaultValue(%s, %s)' % (key, ', '.join(lua_val(float(v)) for v in vals)))
        if nm in self.cp_config: self.cp_used.add(nm)

    def cp_only(self):
        """Script that applies the CP config to an already built .mcn (no rebuild needed)."""
        self.lines = []
        self.prologue('_cparams.log')
        self.emit('TRY("mcn.open", function() return mcn.open(ROOT .. "\\\\" .. NAME .. ".mcn") end)')
        for n in sorted(self.cps, key=lambda n: n.id):
            if self.short(n) in self.cp_config:
                self.cp_settings(n, lua_str(n.name), force=True)
        self.emit('TRY("mcn.save", function() return mcn.save() end)')
        self.emit('LOG("DONE failures=" .. NFAIL)')
        self.emit('LOGF:close()')
        return self.write_lua('_cparams.lua')

    def write_cp_template(self, path):
        """Template listing every CP of this network with its export default; fill in min/max."""
        import json
        data = {}
        if os.path.exists(path):
            data = json.load(open(path, encoding='utf-8'))
        for n in sorted(self.cps, key=lambda n: n.id):
            nm = self.short(n)
            if nm in data:
                data[nm].setdefault('group', None)   # older entries: add the field, keep values
                continue
            d = [float(v) for v in self.xml_default(n)]
            data[nm] = {'type': CP_TYPES[n.type], 'group': None, 'min': None, 'max': None, 'default': d if len(d) > 1 else d[0]}
        with open(path, 'w', encoding='utf-8') as f:
            json.dump(data, f, indent=2)
        return len(data)

    def graph(self):
        nodes = self.nodes
        self.emit('-- 4. graph: state machines, blend tree states, blend tree nodes')
        # ordered creation: parents before children (by path depth)
        items = []
        for i in self.sm_ids: items.append((nodes[i].name, 'SM', nodes[i]))
        for p in self.bt_states: items.append((p, 'BT', None))
        for n in self.graph_nodes: items.append((n.name, 'NODE', n))
        items.sort(key=lambda t: (t[0].count('|'), t[0]))
        for path, kind, n in items:
            comps = path.split('|')
            parent = '|'.join(comps[:-1])
            pexpr = 'P[%s]' % lua_str(parent) if parent else '""'
            key = 'P[%s]' % lua_str(path)
            if kind == 'SM':
                self.call('create StateMachine %s' % path, 'create("StateMachine", %s, %s)' % (pexpr, lua_str(comps[-1])), key)
            elif kind == 'BT':
                self.call('create BlendTree %s' % path, 'create("BlendTree", %s, %s)' % (pexpr, lua_str(comps[-1])), key)
            else:
                self.call('create %s %s' % (NODE_TYPES[n.type], path), 'create(%s, %s, %s)' % (lua_str(NODE_TYPES[n.type]), pexpr, lua_str(comps[-1])), key)
        self.emit()

        self.emit('-- 5. node attributes')
        for n in sorted(self.graph_nodes, key=lambda n: n.id):
            getattr(self, 'attrs_' + NODE_TYPES[n.type])(n, 'P[%s]' % lua_str(n.name))
        self.emit()

        self.emit('-- 6. connections')
        # root of the network
        r = nodes[self.root_id]
        self.emit('TRYANY("connect network root", { %s })' % ', '.join(
            'function() return connect(P[%s] .. ".Result", %s) end' % (lua_str(r.name), lua_str(t)) for t in ('.Result', 'Result', '|Result', 'Network.Result')))
        for n in sorted(self.graph_nodes, key=lambda n: n.id):
            me = 'P[%s]' % lua_str(n.name)
            par = nodes.get(n.parent)
            # output: into parent node pin, or into the BlendTree state's Result when parent is the SM
            if par is not None and par.type == SM_TYPE:
                bt = '|'.join(n.name.split('|')[:-1])
                self.call('connect %s -> %s.Result' % (self.short(n), bt), 'connect(%s .. ".Result", P[%s] .. ".Result")' % (me, lua_str(bt)))
            # inputs
            for field, pin in INPUT_PINS[NODE_TYPES[n.type]].items():
                src = n.get(field)
                if src is None or src in (0xFFFFFFFF, 0xFFFF, -1): continue
                s = nodes.get(src)
                if s is None: continue
                sp = 'P[%s]' % lua_str(s.name)
                if s.type in CP_TYPES:
                    self.emit('TRYANY(%s, { function() return connect(%s .. ".Result", %s .. ".%s") end, function() return connect(%s, %s .. ".%s") end })' % (
                        lua_str('connect CP %s -> %s.%s' % (self.short(s), self.short(n), pin)), sp, me, pin, sp, me, pin))
                else:
                    self.call('connect %s -> %s.%s' % (self.short(s), self.short(n), pin), 'connect(%s .. ".Result", %s .. ".%s")' % (sp, me, pin))
        self.emit()

    # ---- per node type attribute inversion (mirrors manifest serialize())
    def set_attr(self, me, attr, val, per_set=False, desc=None):
        sset = ', SET' if per_set else ''
        self.call(desc or 'setAttribute .%s' % attr, 'setAttribute(%s .. ".%s", %s%s)' % (me, attr, lua_val(val), sset))

    def attrs_AnimWithEvents(self, n, me):
        idx = n.get('AnimIndex')
        if self.lib.sets and idx in self.lib.sets[0][1]:
            e = self.lib.sets[0][1][idx]
            take = {k: e[k] for k in ('filename', 'takename', 'synctrack', 'format', 'options')}
            self.set_attr(me, 'AnimationTake', take, True, 'AnimationTake %s' % self.short(n))
        else:
            self.unsupported.append('anim index %r of %s not in library' % (idx, n.name))
        for a in ('Loop', 'PlayBackwards', 'GenerateAnimationDeltas', 'PreComputeSyncEventTracks'):
            self.set_attr(me, a, bool(n.get(a, False)))
        self.set_attr(me, 'DefaultClip', bool(n.get('DefaultClip_1', True)), True)
        self.set_attr(me, 'ClipStartFraction', float(n.get('ClipStartFraction_1', 0.0)), True)
        self.set_attr(me, 'ClipEndFraction', float(n.get('ClipEndFraction_1', 1.0)), True)
        self.set_attr(me, 'StartEventIndex', int(n.get('StartEventIndex_1', 0)), True)

    def attrs_HeadLook(self, n, me):
        for a in ('UpdateTargetByDeltas', 'ApplyJointLimits', 'MinimiseRotation', 'KeepUpright', 'WorldSpaceTarget'):
            self.set_attr(me, a, bool(n.get(a, False)))
        for a in ('PointingVectorX', 'PointingVectorY', 'PointingVectorZ', 'Bias'):
            self.set_attr(me, a, float(n.get(a + '_1', 0.0)), True)
        for a in ('EndEffectorOffsetX', 'EndEffectorOffsetY', 'EndEffectorOffsetZ'):
            v = float(n.get(a + '_1', 0.0))
            # serialize applies mcn.convertWorldToPhysics(); invert it with the project's scale
            self.call('setAttribute .%s' % a, 'setAttribute(%s .. ".%s", %s / (mcn.convertWorldToPhysics(1.0) or 1.0), SET)' % (me, a, lua_val(v)))
        for a, f in (('EndJointName', 'EndJointIndex_1'), ('RootJointName', 'RootJointIndex_1')):
            j = n.get(f)
            if j in self.joints:
                self.set_attr(me, a, self.joints[j], True)
            else:
                self.unsupported.append('%s joint index %r has no rig name' % (n.name, j))

    def plan_transitions(self):
        """Decide source state / ActiveState for every transition and where its conditions live."""
        nodes = self.nodes
        cond_src = {}   # transit id -> list of (holder node, indices, common?)
        for h in nodes.values():
            for t, idx in h.condsets: cond_src.setdefault(t, []).append((h, idx, False))
            for t, idx in h.common_condsets: cond_src.setdefault(t, []).append((h, idx, True))
        self.active_states = {}  # (sm path, name) -> (all?, [state paths])
        self.tplan = []          # (transit, source path, dest state path, holders, from_active_state)
        for t in sorted(self.transits, key=lambda n: n.id):
            sm = nodes[t.parent]
            tname = self.short(t)
            dst_state = self.state_of.get(t.get('DestNodeID'))
            src = t.get('SourceNodeID')
            holders = cond_src.get(t.id, [])
            if src is not None:
                self.tplan.append((t, self.state_of[src], dst_state, holders, False)); continue
            # No source: a transition from an ActiveState.  Common condition sets mean "all states",
            # otherwise the states that carry the condition set are the ActiveState's subset.
            common = any(c for _, _, c in holders)
            as_name = tname.split('_')[0] if tname.startswith('ActiveState') else (
                'ActiveState' if common else 'ActiveStateSubset%d' % t.id)
            states = sorted({self.state_of[h.id] for h, _, c in holders if not c and h.id in self.state_of})
            key = (sm.name, as_name)
            prev = self.active_states.get(key)
            if prev and (prev[0] != common or (not common and prev[1] != states)):
                as_name = '%s_%d' % (as_name, t.id); key = (sm.name, as_name)
            self.active_states[key] = (common, states)
            self.tplan.append((t, sm.name + '|' + as_name, dst_state, holders, True))

    def transitions(self, active):
        """Emit transitions: active=False -> state to state (stage 1), True -> from ActiveStates (stage 2).
        create() rejects "ActiveState", so --inject adds those nodes to the stage-1 .mcn in between."""
        self.emit('-- 7. transitions and conditions (%s)' % ('from ActiveStates' if active else 'state to state'))
        for t, src, dst_state, holders, from_as in self.tplan:
            if from_as != active: continue
            tkey = 'P[%s]' % lua_str('#t%d' % t.id)
            tname = self.short(t)
            args = '%s, %s, %s' % (lua_str(src), lua_str(dst_state), lua_str(tname))
            self.emit('%s = TRYANY(%s, { function() return create("Transit", %s) end, function() return createTransition("Transit", %s) end })' % (
                tkey, lua_str('create Transit #%d %s' % (t.id, tname)), args, args))
            self.attrs_Transit(t, tkey)
            if holders:
                h, idx, common = holders[0]
                table = h.common_conditions if common else h.conditions
                for k, ci in enumerate(idx):
                    self.condition(table[ci], tkey, '#t%d.c%d' % (t.id, k))
        self.emit()

    def attrs_Transit(self, t, me):
        g = t.get
        self.set_attr(me, 'DurationInTime', float(g('DurationInTime', 0.0)))
        for a in ('AdditiveBlendAttitude', 'AdditiveBlendPosition', 'SphericallyInterpolateTrajectoryPosition',
                  'DeadblendBreakoutToSource', 'UseDestinationStartFraction', 'UseDestinationStartSyncEventIndex',
                  'UseDestinationStartSyncEventFraction', 'BreakoutTransit', 'FreezeSource', 'FreezeDest',
                  'UseDeadReckoningWhenDeadBlending', 'BlendToDestinationPhysicsBones'):
            if g(a) is not None: self.set_attr(me, a, bool(g(a)))
        self.set_attr(me, 'DestinationStartFraction', float(g('DestinationStartFraction', 0.0)))
        ev = float(g('DestinationStartSyncEvent', 0.0))
        idx, frac = math.floor(ev), ev - math.floor(ev)
        if not g('UseDestinationStartSyncEventIndex'): idx = 0.0
        self.set_attr(me, 'DestinationStartSyncEventIndex', float(idx))
        self.set_attr(me, 'DestinationStartSyncEventFraction', float(frac))
        dts = int(g('DeltaTrajSource', 3))
        self.set_attr(me, 'DeltaTrajSource', dts if dts in (1, 2) else 3)   # 0/3 both mean "blend both"
        if g('isReversibleTransit'):
            self.set_attr(me, 'ReversibleTransit', True)
            cp = self.nodes.get(g('RuntimeNodeID'))
            if cp is not None:
                self.call('ReverseControlParameter', 'setAttribute(%s .. ".ReverseControlParameter", P[%s])' % (me, lua_str(cp.name)))
        if int(g('DestinationSubStateCount', 0) or 0) > 0:
            # deepest sub state given by the last DestinationSubStateID_*
            sub = g('DestinationSubStateID_%d' % (int(g('DestinationSubStateCount')) - 1))
            sub_node = self.nodes.get(sub)
            if sub_node is not None and sub_node.id in self.state_of:
                self.call('DestinationSubState', 'setAttribute(%s .. ".DestinationSubState", P[%s])' % (me, lua_str(self.state_of[sub_node.id])))
            else:
                self.unsupported.append('transition %s destination sub state %r' % (t.name, sub))

    def condition(self, c, tkey, label):
        ctype = COND_TYPES.get(c.type)
        if ctype is None:
            self.unsupported.append('condition type %d on %s' % (c.type, label)); return
        ckey = 'P[%s]' % lua_str(label)
        self.emit('%s = TRYANY(%s, { function() return create(%s, %s) end, function() return createCondition(%s, %s) end })' % (
            ckey, lua_str('create %s %s' % (ctype, label)), lua_str(ctype), tkey, lua_str(ctype), tkey))
        if c.type == 601:
            self.call('Message', 'setAttribute(%s .. ".Message", %s)' % (ckey, self.req_var.get(c.get('MessageID'), 'nil')))
            self.set_attr(ckey, 'OnNotSet', bool(c.get('OnNotSet')))
        elif c.type == 603:
            self.set_attr(ckey, 'TriggerPercent', float(c.get('TestFraction')))
        elif c.type == 609:
            cp = self.nodes.get(c.get('RuntimeNodeID'))
            if cp is not None:
                self.call('ControlParameter', 'setAttribute(%s .. ".ControlParameter", P[%s])' % (ckey, lua_str(cp.name)))
            self.set_attr(ckey, 'LowerTestValue', float(c.get('LowerTestValue')))
            self.set_attr(ckey, 'UpperTestValue', float(c.get('UpperTestValue')))
            self.set_attr(ckey, 'NotInRange', bool(c.get('NotInRange')))
        elif c.type == 611:
            self.set_attr(ckey, 'EventRangeStart', float(c.get('EventRangeStart')))
            self.set_attr(ckey, 'EventRangeEnd', float(c.get('EventRangeEnd')))
        else:
            self.unsupported.append('condition %s attributes not mapped (%s)' % (ctype, label))

    def default_states(self):
        self.emit('-- 8. default states')
        for i in self.sm_ids:
            sm = self.nodes[i]
            d = sm.get('DefaultNodeID')
            st = self.state_of.get(d)
            if st: self.call('setDefaultState %s' % sm.name, 'setDefaultState(P[%s], P[%s])' % (lua_str(sm.name), lua_str(st)))
        self.emit()

    def layout(self):
        """Editor layout is not in the export; lay states out on a grid and blend trees right-to-left."""
        self.emit('-- 9. layout (synthetic: original editor positions are not in the export)')
        nodes = self.nodes
        for i in self.sm_ids:
            sm = nodes[i]
            states = sorted({s for s in self.state_of.values() if s.rsplit('|', 1)[0] == sm.name})
            cols = max(1, int(math.ceil(math.sqrt(len(states)))))
            for k, s in enumerate(states):
                self.call('pos %s' % s, 'setNodePosition(P[%s], %d, %d)' % (lua_str(s), 300 * (k % cols), 160 * (k // cols)))
        for bt in self.bt_states:
            inside = [n for n in self.graph_nodes if n.name.rsplit('|', 1)[0] == bt]
            depth = {}
            def d(n):
                if n.id in depth: return depth[n.id]
                p = nodes.get(n.parent)
                depth[n.id] = 0 if (p is None or p.type == SM_TYPE) else d(p) + 1
                return depth[n.id]
            for k, n in enumerate(sorted(inside, key=lambda n: n.id)):
                self.call('pos %s' % self.short(n), 'setNodePosition(P[%s], %d, %d)' % (lua_str(n.name), -280 * d(n), 120 * k))
        self.emit()

    def epilogue(self):
        self.emit('-- 10. save and re-export for round-trip verification')
        self.emit('TRY("mcn.saveAs", function() return mcn.saveAs(ROOT .. "\\\\" .. NAME .. ".mcn") end)')
        self.emit('pcall(function() app.createDirectory(ROOT .. "\\\\roundtrip") end)')
        self.emit('local ok, res, ids, errors, warnings = pcall(mcn.export, ROOT .. "\\\\roundtrip\\\\" .. NAME .. ".xml")')
        self.emit('LOG("export ok=" .. tostring(ok) .. " result=" .. TS(res))')
        self.emit('if type(errors) == "table" then for i, v in ipairs(errors) do LOG("EXPORT ERROR " .. TS(v.name) .. " : " .. TS(v.message)) end end')
        self.emit('if type(warnings) == "table" then for i, v in ipairs(warnings) do LOG("EXPORT WARN " .. TS(v.name) .. " : " .. TS(v.message)) end end')
        self.emit('LOG("DONE failures=" .. NFAIL)')
        self.emit('LOGF:close()')

    def write_project(self):
        set_name = self.set_name
        mcp = f'''<?xml version="1.0" encoding="UTF-8"?>
<NaturalMotion loaderVersion="2" typeString="ConnectProject" productVersion="3.5" library="NMDatabase2">
	<Modules>
		<Module id="nmx" dataVersion="1"/>
		<Module id="mcc" dataVersion="36"/>
	</Modules>
	<Node name="Selection" type="SelectionNode" module="nmx">
		<Bool name="Renameable" val="0"/>
		<Bool name="Deletable" val="0"/>
	</Node>
	<Node name="GlobalSettings" type="MorphemeProjectSettingsNode" module="mcc">
		<Bool name="Renameable" val="0"/>
		<Bool name="Deletable" val="0"/>
		<Enum name="PhysicsEngine" val="PhysX3"/>
		<Enum name="WorldUpAxis" val="Z Axis"/>
		<Enum name="ViewportRenderUnit" val="Metres"/>
		<Float name="RuntimeAssetScaleFactor" val="1"/>
		<Node name="Defaults" type="MorphemeNetworkDefaultsSettingsNode" module="mcc">
			<Bool name="Renameable" val="0"/>
			<Bool name="Deletable" val="0"/>
			<Node name="AnimationLocation" type="AnimationLocation" module="mcc">
				<String name="SourceDirectory" val="$(RootDir)\\motion_xmd"/>
				<String name="MarkupDirectory" val="$(RootDir)\\morphemeMarkup"/>
			</Node>
			<Node name="{set_name}" type="AnimationSet" module="mcc">
				<String name="AnimationRig" val="$(RootDir)\\{set_name}.mcarig"/>
				<String name="Format" val="nsa"/>
			</Node>
		</Node>
	</Node>
	<Node name="EuphoriaSettings" type="EuphoriaProjectSettingsNode" module="mcc">
		<Bool name="Renameable" val="0"/>
		<Bool name="Deletable" val="0"/>
	</Node>
	<Node name="KinectSettings" type="KinectProjectSettingsNode" module="mcc">
		<Bool name="Renameable" val="0"/>
		<Bool name="Deletable" val="0"/>
	</Node>
</NaturalMotion>
'''
        with open(os.path.join(self.out_dir, self.name + '.mcp'), 'w', newline='\r\n') as f: f.write(mcp)

    def stage2_prologue(self):
        self.emit('TRY("mcn.open", function() return mcn.open(ROOT .. "\\\\" .. NAME .. ".mcn") end)')
        self.emit('local SET = %s' % lua_str(self.set_name))
        self.emit('TRY("setSelectedAnimSet", function() return setSelectedAnimSet(SET) end)')
        # stage 1 got back exactly the requested names, so the paths can be rebuilt here
        for m in self.msgs:
            self.emit('P[%s] = %s' % (lua_str('#msg%s' % m['messageID']), lua_str('Requests|' + m['name'])))
        for n in sorted(self.nodes.values(), key=lambda n: n.id):
            if n.name: self.emit('P[%s] = %s' % (lua_str(n.name), lua_str(n.name)))
        self.emit()
        self.anim_set_options()

    def anim_set_options(self):
        """Per-take options were dropped by setAttribute in testing; when every library entry
        shares one option string, put it on the animation set instead (what the export falls back to)."""
        opts = {e['options'] for e in (self.lib.sets[0][1].values() if self.lib.sets else [])}
        if len(opts) == 1 and list(opts)[0]:
            o = list(opts)[0]
            self.call('anim.setAnimSetOptions', 'anim.setAnimSetOptions(SET, %s)' % lua_str(o))
            self.emit('LOG("animset options=" .. TS(anim.getAnimSetOptions(SET)))')
            first = next(n for n in self.graph_nodes if n.type == 104)
            self.emit('LOG("take=" .. TS(getAttribute(%s, "AnimationTake", SET)))' % lua_str(first.name))
            self.emit()

    def write_lua(self, suffix):
        lua = os.path.join(self.out_dir, self.name + suffix)
        with open(lua, 'w', newline='\r\n') as f: f.write('\n'.join(self.lines) + '\n')
        return lua

    def run(self):
        self.analyse(); self.plan_transitions()
        self.prologue(); self.setup(); self.requests(); self.control_params(); self.graph(); self.anim_set_options()
        self.transitions(False); self.default_states(); self.layout(); self.epilogue()
        self.write_project()
        out = [self.write_lua('_rebuild.lua')]
        if self.active_states:
            self.lines = []
            self.prologue('_stage2.log'); self.stage2_prologue()
            self.transitions(True); self.epilogue()
            out.append(self.write_lua('_stage2.lua'))
        return out

    # ------------------------------------------------------------------ .mcn injection
    def inject(self, mcn_path):
        """Add the ActiveState state machine nodes to the stage-1 .mcn (format as in Connect's samples)."""
        tree = ET.parse(mcn_path)
        net = tree.getroot().find('MorphemeDB/Networks/Network')

        def graph_of(elem, ptr):
            g = list(elem.find('GraphEntry'))[0]          # the BlendTree / StateMachine graph
            return g, '%s.GraphEntry.%s' % (ptr, g.get('name'))

        def locate_graph(path):
            """Return (graph element, pointer) of the graph owned by the object at a Connect path."""
            g, ptr = graph_of(net, 'MorphemeDB.Networks.Network')
            for comp in path.split('|'):
                for cont in ('BlendTreeNodes', 'StateMachineNodes'):
                    c = g.find(cont)
                    hit = c.find('*[@name="%s"]' % comp) if c is not None else None
                    if hit is not None: break
                else:
                    raise KeyError('%s: %s not found' % (path, comp))
                g, ptr = graph_of(hit, '%s.%s.%s' % (ptr, cont, comp))
            return g, ptr

        added = 0
        for (smname, as_name), (allstates, states) in sorted(self.active_states.items()):
            g, gptr = locate_graph(smname)
            container = g.find('StateMachineNodes')
            if container.find('*[@name="%s"]' % as_name) is not None: continue
            n = ET.SubElement(container, 'StateMachineNode', name=as_name, type='node')
            at = ET.SubElement(n, 'Attributes', type='nodeContainer')
            b = ET.SubElement(at, 'BoolAttribute', name='AllStates', type='node')
            ET.SubElement(b, 'Value', type='bool').text = '1' if allstates else '0'
            ra = ET.SubElement(at, 'RefArrayAttribute', name='States', type='node')
            if states and not allstates:
                v = ET.SubElement(ra, 'Value', type='attributeArray', size=str(len(states)), elemType='pointer')
                for st in states:
                    ET.SubElement(v, 'elem', type='pointer').text = '%s.StateMachineNodes.%s' % (gptr, st.split('|')[-1])
            for tag, val in (('XPosition', -400.0), ('YPosition', 90.0 * added), ('Width', 210.0), ('Height', 50.3984375)):
                ET.SubElement(n, tag, type='float').text = repr(val)
            ET.SubElement(n, 'ManifestVersion', type='int').text = '1'
            ET.SubElement(n, 'NodeType', type='string').text = 'ActiveState'
            added += 1
        groups = self.inject_cp_groups(net)
        ET.indent(tree, '\t')
        with open(mcn_path, 'wb') as f:
            f.write(b'<?xml version="1.0" encoding="UTF-8"?>\r\n')
            f.write(ET.tostring(tree.getroot(), encoding='utf-8').replace(b'\n', b'\r\n'))
        return added, groups

    def inject_cp_groups(self, net):
        """Put CPs into ControlParameterGroups from the config's "group" field (no Lua API exists for
        CP groups; layout as in Connect's samples).  Returns the number of groups written."""
        wanted = {}   # group -> [cp names] in network order
        for n in sorted(self.cps, key=lambda n: n.id):
            g = (self.cp_config.get(self.short(n)) or {}).get('group')
            if g: wanted.setdefault(g, []).append(self.short(n))
        if not wanted: return 0
        cpnode = net.find('ControlParametersNode')
        arr = cpnode.find('ControlParameterArray')
        existing = {c.get('name') for c in arr.findall('ControlParameter')}
        container = cpnode.find('ControlParametersNode')
        if container is None:
            container = ET.Element('ControlParametersNode', type='nodeContainer')
            cpnode.insert(list(cpnode).index(arr) + 1, container)
        regrouped = {cp for cps in wanted.values() for cp in cps}
        ptr = 'MorphemeDB.Networks.Network.ControlParameters.ControlParameterArray.'
        # a CP lives in one group: drop regrouped CPs from groups they are already in
        for grp in container.findall('ControlParameterGroup'):
            ga = grp.find('ControlParameterArray')
            if ga is None: continue
            for e in list(ga):
                if e.text and e.text[len(ptr):] in regrouped: ga.remove(e)
            ga.set('size', str(len(ga)))
        for gname, cps in wanted.items():
            grp = container.find('ControlParameterGroup[@name="%s"]' % gname)
            if grp is None:
                grp = ET.SubElement(container, 'ControlParameterGroup', name=gname, type='node')
            ga = grp.find('ControlParameterArray')
            if ga is None:
                ga = ET.SubElement(grp, 'ControlParameterArray', type='attributeArray', elemType='pointer')
            for cp in cps:
                if cp not in existing:
                    self.unsupported.append('CP group %s: %s is not in the .mcn' % (gname, cp)); continue
                ET.SubElement(ga, 'elem', type='pointer').text = ptr + cp
            ga.set('size', str(len(ga)))
        for grp in container.findall('ControlParameterGroup'):   # drop groups left empty
            ga = grp.find('ControlParameterArray')
            if ga is None or len(ga) == 0: container.remove(grp)
        return len(wanted)

    def inject_groups_only(self, mcn_path):
        tree = ET.parse(mcn_path)
        n = self.inject_cp_groups(tree.getroot().find('MorphemeDB/Networks/Network'))
        if n:
            ET.indent(tree, '\t')
            with open(mcn_path, 'wb') as f:
                f.write(b'<?xml version="1.0" encoding="UTF-8"?>\r\n')
                f.write(ET.tostring(tree.getroot(), encoding='utf-8').replace(b'\n', b'\r\n'))
        return n


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('xml')
    ap.add_argument('--library'); ap.add_argument('--rig'); ap.add_argument('--model'); ap.add_argument('--out-dir')
    ap.add_argument('--inject', action='store_true', help='add ActiveState nodes to the stage-1 .mcn (between the two Connect passes)')
    ap.add_argument('--cp-config', default=DEFAULT_CP_CONFIG,
                    help='JSON of control parameter settings by name: {"Name": {"min": x, "max": y, "default": z or [x, y, z]}}')
    ap.add_argument('--cp-template', action='store_true', help='add the CPs of this network (export defaults, min/max null) to --cp-config')
    ap.add_argument('--cp-only', action='store_true', help='write <name>_cparams.lua that applies --cp-config to the existing .mcn')
    a = ap.parse_args()
    import json
    cp_config = {}
    if a.cp_config and os.path.exists(a.cp_config) and not a.cp_template:
        cp_config = {k: v for k, v in json.load(open(a.cp_config, encoding='utf-8')).items() if not k.startswith('_')}
    base = os.path.dirname(os.path.abspath(a.xml))
    name = os.path.splitext(os.path.basename(a.xml))[0]
    lib = Library(a.library or os.path.join(base, name + '_Library.xml'))
    rig = a.rig or next((os.path.join(base, f) for f in os.listdir(base) if f.endswith('.mrarig')), None)
    joints, rig_root = load_rig_joints(rig) if rig else ({}, None)
    model = a.model or (os.path.join(base, name + '.xmd') if os.path.exists(os.path.join(base, name + '.xmd')) else None)
    c = Converter(a.xml, lib, joints, rig_root, os.path.abspath(a.out_dir or base), model, cp_config)
    if a.cp_template:
        c.analyse()
        print('%s now lists %d control parameters' % (a.cp_config, c.write_cp_template(a.cp_config)))
        return
    luas = c.run()
    if a.cp_only:
        mcn = os.path.join(c.out_dir, c.name + '.mcn')
        print('wrote %d CP groups into %s' % (c.inject_groups_only(mcn), mcn))
        print('wrote', c.cp_only()); return
    if a.inject:
        mcn = os.path.join(c.out_dir, c.name + '.mcn')
        n_as, n_grp = c.inject(mcn)
        print('injected %d ActiveState nodes and %d CP groups into %s' % (n_as, n_grp, mcn))
        for u in c.unsupported: print('UNSUPPORTED:', u)
        return
    for lua in luas: print('wrote', lua)
    print('script lines:', len(c.lines))
    for u in c.unsupported: print('UNSUPPORTED:', u)
    if cp_config:
        print('CP config: %d entries, %d used by this network' % (len(cp_config), len(c.cp_used)))


if __name__ == '__main__':
    main()
