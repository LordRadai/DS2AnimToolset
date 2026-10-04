"""
xml2mcn.py - rebuild a morphemeConnect 3.6 network (.mcn) from a compiled network export XML.

The export XML (NetworkDefinition/Node/DataBlock) is what Connect's per-node Lua serialize()
functions write.  This tool inverts those serializers (nodespecs.py) and emits Lua scripts that
recreate the network through Connect's own scripting API, so Connect writes the .mcn itself.

Every export name is a Connect path ("SM_Main|BT_BaseAct|Blend2_704"), so the hierarchy is rebuilt
component by component: a path prefix that is not itself a node is a BlendTree state when its
parent is a state machine, otherwise a nested BlendTree node.  Connections that cross graph levels
(multiply connected nodes) are routed through one-to-many pass-down pins on every container in
between, the way Connect itself stores them.

Workflow (create() cannot make ActiveState nodes or pass-down pins on states, hence two passes):
  1. python xml2mcn.py X.xml                  -> X.mcp, X_rebuild.lua, X_stage2.lua
  2. morphemeConnect.exe -nogui -script X_rebuild.lua
  3. python xml2mcn.py X.xml --inject         -> ActiveStates, state pass-down pins, CP groups into X.mcn
  4. morphemeConnect.exe -nogui -script X_stage2.lua
  5. python xmldiff.py X.xml roundtrip\\X.xml

Control parameters: tools\\ds2_control_parameters.json (or --cp-config) sets min/max/default/group by
CP name; its default wins over the export's.  --cp-template adds a network's CPs to that file,
--cp-only writes <name>_cparams.lua (and the groups) for an existing .mcn without rebuilding.
"""
import argparse, json, math, os, re, sys, xml.etree.ElementTree as ET
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mcnxml, nodespecs, manifest_info

CP_TYPES = {20: 'float', 21: 'vector3', 22: 'vector4', 23: 'bool', 24: 'int', 25: 'uint'}
VECTOR_CP_TYPES = (21, 22)
SM_TYPE, NETWORK_TYPE = 10, 9
TRANSIT_TYPES = {402: 'Transit', 400: 'TransitMatchEvents'}
DEFAULT_CP_CONFIG = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'cp_config.json')
INVALID = (0xFFFFFFFF, 0xFFFF, -1)
LATE_ATTRS = ('SourceWeightDistribution', 'SourceWeights')


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
        self.event_mirrors, self.track_mirrors = [], []   # per set
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
            # left/right event userdata pairs MirrorTransforms swaps
            self.event_mirrors.append([(int(m.get('first')), int(m.get('second'))) for m in s.findall('EventMirrorMapping')])
            self.track_mirrors.append([(m.get('first'), m.get('second')) for m in s.findall('EventTrackMirrorMapping')])


def load_rig_joints(path):
    if not path or not os.path.exists(path): return {}, None
    r = ET.parse(path).getroot()
    return {int(j.get('index')): j.get('name') for j in r.iter('Joint')}, r


def parent_path(p): return p.rsplit('|', 1)[0] if '|' in p else ''


class Converter:
    def __init__(self, xml_path, lib, joints, rig_root, out_dir, model, cp_config=None):
        self.cp_config, self.cp_used = cp_config or {}, set()
        self.root, self.nodes, self.msgs = mcnxml.load(xml_path)
        self.name = self.root.get('name') or os.path.splitext(os.path.basename(xml_path))[0]
        # node names come only from NodeIDNamesTable.xml next to the export (the export's own names are ignored)
        self.state_names = {}   # (state machine id, state root id) -> path of the container holding the root
        tbl = os.path.join(os.path.dirname(os.path.abspath(xml_path)), 'NodeIDNamesTable.xml')
        if os.path.exists(tbl):
            t = ET.parse(tbl).getroot()
            names = {int(e.get('NodeID')): e.get('Name') or '' for e in t.findall('Node')}
            for n in self.nodes.values(): n.name = names.get(n.id, '')
            for e in t.findall('StateNode'):
                if e.get('Name'):
                    for pe in e.findall('ParentNode'): self.state_names[(int(pe.get('NodeID')), int(e.get('NodeID')))] = e.get('Name')
        self.lib, self.joints, self.rig_root, self.out_dir, self.model = lib, joints, rig_root, out_dir, model
        # generation-only files (scripts, logs, paths, round-trip export) stay out of the project folder
        self.build_dir = os.path.join(out_dir, 'build')
        os.makedirs(self.build_dir, exist_ok=True)
        self.set_name = lib.sets[0][0] if lib.sets else 'AnimSet'
        self.set_names = [s[0] for s in lib.sets] or [self.set_name]
        # each animation set has its own <set>.mrarig next to the export (DS2 sets share one skeleton)
        self.set_rigs = []
        for sn in self.set_names:
            sj, sr = load_rig_joints(os.path.join(os.path.dirname(os.path.abspath(xml_path)), sn + '.mrarig'))
            self.set_rigs.append((sj, sr) if sr is not None else (joints, rig_root))
        self.lines, self.unsupported, self.unmapped = [], [], set()
        self.manifest = manifest_info.load()
        self.nchannels = len(joints)
        self.joint_parent = {int(j.get('index')): int(j.get('parent')) for j in rig_root.iter('Joint')} if rig_root is not None else {}

    # ------------------------------------------------------------------ spec context
    def anim_take(self, idx):
        if self.lib.sets and idx in self.lib.sets[0][1]:
            e = self.lib.sets[0][1][idx]
            return {k: e[k] for k in ('filename', 'takename', 'synctrack', 'format', 'options')}
        return None

    # ------------------------------------------------------------------ structure
    def analyse(self):
        nodes = self.nodes
        self.cps = sorted((n for n in nodes.values() if n.type in CP_TYPES), key=lambda n: n.id)
        self.sms = {n.name: n for n in nodes.values() if n.type == SM_TYPE}
        self.transits = sorted((n for n in nodes.values() if n.type in TRANSIT_TYPES), key=lambda n: n.id)
        self.gnodes = sorted((n for n in nodes.values() if n.type in nodespecs.SPECS), key=lambda n: n.id)
        for n in nodes.values():
            if n.type not in nodespecs.SPECS and n.type not in CP_TYPES and n.type not in TRANSIT_TYPES \
                    and n.type not in (SM_TYPE, NETWORK_TYPE):
                self.unsupported.append('node #%d %s (typeID %d)' % (n.id, n.name, n.type))
        self.root_id = int(self.root.findtext('rootNodeNetworkID'))
        # spec every graph node once
        self.spec = {}
        self.spec_sets = {}   # node id -> {set index >= 2: per-set attrs of that animation set}
        K = len(self.set_names)
        joints0 = self.joints
        for n in self.gnodes:
            if K == 1:
                mtype, attrs, inputs = nodespecs.SPECS[n.type](self, n)
            else:
                for k in range(1, K + 1):
                    self.joints = self.set_rigs[k - 1][0] or joints0
                    mk, ak, ik = nodespecs.SPECS[n.type](self, nodespecs.SetView(n, k, K))
                    if k == 1: mtype, attrs, inputs = mk, ak, ik
                    else: self.spec_sets.setdefault(n.id, {})[k] = [x for x in ak if x[2]]
                self.joints = joints0
            self.spec[n.id] = (mtype, attrs, inputs)
        self.renames = {}   # node id -> original export name, for nodes the decompiler placed where Connect can't wire them
        if any(not n.name for n in nodes.values() if n.type not in CP_TYPES and n.type != NETWORK_TYPE):
            self.recover_hierarchy()
        for n in nodes.values(): n.orig_name = n.name
        self.build_containers()
        if self.wrap_state_machines(): self.build_containers()
        for _ in range(4):
            if not self.fix_placement(): break
            self.build_containers()

    def wrap_state_machines(self):
        """A multiply connected node N whose runtime parent is state machine X, but which the decompiler put
        inside a state of SM Y (Y itself a state of X), originally lived in a BlendTree state of X next to Y,
        feeding Y through a pass-down pin (the requester of a one-to-many pin is whatever Y's result feeds,
        here X).  Rebuild that state: X|Y -> X|BlendTree_<Y>_0|Y, and move N's side chain out of Y's state."""
        nodes, changed = self.nodes, False
        gen = lambda leaf: bool(re.fullmatch(r'[A-Za-z0-9]+_\d+(_\d+)?', leaf)) or bool(re.fullmatch(r'a\d\d_\d\d_\d{4}_.*', leaf))
        for n in sorted(self.gnodes, key=lambda n: n.id):
            if n.attrs.get('downstreamMultiplyConnected') != 'true': continue
            X = nodes.get(n.parent)
            if X is None or X.type != SM_TYPE or not n.name.startswith(X.name + '|'): continue
            rest = n.name[len(X.name) + 1:].split('|')
            wrapped = False
            m0 = re.fullmatch(r'BlendTree_(\d+)_0', rest[0])
            if m0 and len(rest) >= 4 and (X.name + '|' + rest[0] + '|' + rest[1]) in self.sms and                     self.sms[X.name + '|' + rest[0] + '|' + rest[1]].id == int(m0.group(1)):
                wrapped, rest = True, rest[1:]           # Y already moved into its wrapper by an earlier node
                Y = self.sms[X.name + '|' + m0.group(0) + '|' + rest[0]]
            else:
                Y = self.sms.get(X.name + '|' + rest[0])
            if Y is None or len(rest) < 3: continue
            S = Y.name + '|' + rest[1]                     # state of Y holding N
            if not self.is_state(S) or S not in self.croot: continue
            W = '%s|BlendTree_%d_0' % (X.name, Y.id)
            under = [m for m in nodes.values() if m.name == Y.name or m.name.startswith(Y.name + '|')]
            if not wrapped:
                if any(not gen(c) for m in under if m.type not in TRANSIT_TYPES for c in m.name[len(X.name) + 1:].split('|')): continue
                if any(m.name.startswith(W + '|') for m in nodes.values()): continue
            # containers from S down to the one holding S's root node; everything else in them is side chain
            chain, g = [], parent_path(nodes[self.croot[S]].name)
            while g != S and g.startswith(S + '|'): chain.append(g); g = parent_path(g)
            chain.append(S)
            if parent_path(n.name) not in chain: continue
            # move N plus the part of its upstream that feeds nothing else
            cons = self.consumers()
            side, todo = {n.id}, [n]
            while todo:
                m = todo.pop()
                for field, pin in self.spec[m.id][2]:
                    u = nodes.get(m.get(field))
                    if u is None or u.id in side or u.type not in nodespecs.SPECS or parent_path(u.name) not in chain: continue
                    if u.id == self.croot[S] or not all(c.id in side for c in cons.get(u.id, [])): continue
                    side.add(u.id); todo.append(u)
            side = [nodes[i] for i in side]
            if not wrapped:
                for m in under:
                    self.renames.setdefault(m.id, m.orig_name)
                    m.name = W + m.name[len(X.name):]
            for m in side:
                self.renames.setdefault(m.id, m.orig_name)
                m.name = W + '|' + m.name.split('|')[-1]
            changed = True
            self.build_containers()
        return changed

    def recover_hierarchy(self):
        """Export without decompiler names: place every node from the runtime data (hierarchy.py), keep the
        NMB names exactly and name AnimWithEvents after their animation."""
        import hierarchy
        lib = self.lib.sets[0][1] if self.lib.sets else {}
        def anim_name(n):
            e = lib.get(n.get('AnimIndex'))
            return os.path.splitext(re.split(r'[\\/]', e['filename'])[-1])[0] if e else None
        named = {i: n.name for i, n in self.nodes.items() if n.name and n.type not in CP_TYPES and n.type != NETWORK_TYPE}
        h = hierarchy.Hierarchy(self.nodes, self.root_id, self.spec, anim_name)
        paths = h.build(named, self.state_names)
        for i, p in paths.items():
            if i in named and named[i] != p: self.unsupported.append('named node %d %s placed as %s' % (i, named[i], p))
            self.nodes[i].name = named.get(i, p)
        for l in h.log: self.unsupported.append('hierarchy: ' + l)
        self.recovered = True

    def build_containers(self):
        """containers: path -> 'sm' | 'state' | 'nbt'   ('' is the root blend tree)"""
        self.sms = {n.name: n for n in self.nodes.values() if n.type == SM_TYPE}
        self.containers = {'': 'root'}
        for p in self.sms: self.containers[p] = 'sm'
        for n in list(self.gnodes) + list(self.sms.values()):
            comps = n.name.split('|')
            for k in range(1, len(comps)):
                p = '|'.join(comps[:k])
                if p in self.containers: continue
                self.containers[p] = 'state' if parent_path(p) in self.sms else 'nbt'
        self.find_roots()

    def consumers(self):
        cons = {}
        for n in self.gnodes:
            for field, pin in self.spec[n.id][2]:
                sid = n.get(field)
                if sid in self.nodes: cons.setdefault(sid, []).append(n)
        return cons

    def graph_lca(self, graphs):
        def up(p): return [p] + (up(parent_path(p)) if p else [])
        common = None
        for g in graphs:
            chain = up(g)
            common = chain if common is None else [c for c in common if c in chain]
        L = common[0] if common else ''
        while L in self.sms: L = parent_path(L)   # nodes cannot live directly in a state machine graph
        return L

    def fix_placement(self):
        """The decompiler places unnamed data nodes (operators) and some multiply connected nodes inside
        containers they never feed.  Connect can only route a node out of a container through its Result,
        so such nodes move to the lowest graph above all their consumers.  Only generated names
        ('<Type>_<id>') are moved; the move is recorded so xmldiff can match them."""
        moved = False
        cons = self.consumers()
        for n in self.gnodes:
            leaf = n.name.split('|')[-1]
            if not re.fullmatch(r'[A-Za-z0-9]+_%d' % n.id, leaf) or n.id not in cons: continue
            here = parent_path(n.name)
            is_data = n.type in (110, 112, 142, 144, 146)
            target = self.graph_lca([parent_path(c.name) for c in cons[n.id]])
            if is_data:   # fine anywhere above its consumers (reached through pass-down pins)
                ok = here == '' or target == here or target.startswith(here + '|')
            else:
                ok = all(self.can_exit(n, here, parent_path(c.name)) for c in cons[n.id])
            if ok: continue
            self.renames[n.id] = n.orig_name
            n.name = (target + '|' if target else '') + leaf
            moved = True
        return moved

    def can_exit(self, n, gs, gc):
        """Can node n (in graph gs) reach a consumer in graph gc? Every container it leaves must be rooted at n."""
        L = gs
        while not (gc == L or gc.startswith(L + '|') or L == ''): L = parent_path(L)
        g = gs
        while g != L:
            if self.croot.get(g) != n.id: return False
            g = parent_path(g)
        return True

    def is_state(self, p): return self.containers.get(p) == 'state' or (p in self.sms and parent_path(p) in self.sms)

    def state_in(self, sm_path, nid):
        """The state of state machine sm_path that contains runtime node nid."""
        n = self.nodes.get(nid)
        if n is None or not n.name.startswith(sm_path + '|'): return None
        rest = n.name[len(sm_path) + 1:].split('|')
        return sm_path + '|' + rest[0]

    def find_roots(self):
        """Root item (direct child node or container) of every BlendTree container."""
        nodes = self.nodes
        self.croot = {}   # container path -> root node id
        for p, kind in self.containers.items():
            if kind not in ('state', 'nbt', 'root'): continue
            rid = None
            if kind == 'root':
                rid = self.root_id
            else:
                m = re.fullmatch(r'BlendTree_(\d+)_\d+', p.split('|')[-1])
                if m and int(m.group(1)) in nodes and nodes[int(m.group(1))].name.startswith(p + '|'):
                    rid = int(m.group(1))
                elif kind == 'state':
                    sm = self.sms[parent_path(p)]
                    for d, t, v, a in sm.elems:
                        if d.startswith('RuntimeChildNodeID') and v in nodes and nodes[v].name.startswith(p + '|'):
                            rid = v; break
                if rid is None:   # fallback: the node whose downstream parent is outside
                    for n in nodes.values():
                        if n.name.startswith(p + '|') and n.parent in nodes and not nodes[n.parent].name.startswith(p + '|'):
                            rid = n.id; break
            if rid is not None: self.croot[p] = rid
            else: self.unsupported.append('no root found for container %s' % p)

    def item_in(self, graph, nid):
        """The direct child of `graph` (node or container path) that contains node nid."""
        n = self.nodes[nid]
        if graph == '': rest = n.name.split('|')
        else: rest = n.name[len(graph) + 1:].split('|')
        return (graph + '|' if graph else '') + rest[0], len(rest) == 1

    # ------------------------------------------------------------------ Lua helpers
    def emit(self, s=''): self.lines.append(s)

    def call(self, desc, expr, var=None):
        self.emit('%sTRY(%s, function() return %s end)' % ((var + ' = ') if var else '', lua_str(desc), expr))

    def ref(self, key):
        """Lua expression for the Connect path of a node id or container path."""
        return 'P[%s]' % (key if isinstance(key, int) else lua_str(key))

    def prologue(self, log_suffix):
        self.emit('-- Generated by xml2mcn.py from %s. Run: morphemeConnect.exe -nogui -script <this file>' % self.name)
        self.emit('local ROOT = %s' % lua_str(self.out_dir))
        self.emit('local NAME = %s' % lua_str(self.name))
        self.emit('local BUILD = %s' % lua_str(self.build_dir))
        self.emit('local LOGF = io.open(BUILD .. "\\\\" .. NAME .. "%s", "w")' % log_suffix)
        self.emit('NFAIL = 0')
        self.emit('function LOG(s) LOGF:write(s .. "\\n"); LOGF:flush() end')
        self.emit('function TS(v) if type(v) == "table" then local t = {} for k, x in pairs(v) do table.insert(t, tostring(k) .. "=" .. tostring(x)) end return "{" .. table.concat(t, ", ") .. "}" end return tostring(v) end')
        self.emit('function TRY(desc, f)')
        self.emit('  local ok, r1, r2 = pcall(f)')
        self.emit('  if not ok then NFAIL = NFAIL + 1; LOG("FAIL  " .. desc .. "  :: " .. tostring(r1)); return nil end')
        self.emit('  if r1 == nil or r1 == false then NFAIL = NFAIL + 1; LOG("NIL   " .. desc .. "  -> " .. TS(r1) .. " " .. TS(r2)); return r1 end')
        self.emit('  return r1')
        self.emit('end')
        self.emit('function TRYANY(desc, fs)')
        self.emit('  for i, f in ipairs(fs) do local ok, r = pcall(f); if ok and r then return r end end')
        self.emit('  NFAIL = NFAIL + 1; LOG("FAIL  " .. desc .. " (all variants)"); return nil')
        self.emit('end')
        self.emit('-- connections are queued and retried until no more succeed (interfaces form bottom up)')
        self.emit('CONNS = {}')
        self.emit('function CONN(src, dst) if src and dst then table.insert(CONNS, { src, dst }) else NFAIL = NFAIL + 1; LOG("FAIL  connect missing path " .. TS(src) .. " -> " .. TS(dst)) end end')
        self.emit('function RUNCONNS()')
        self.emit('  local pending = CONNS')
        self.emit('  for round = 1, 12 do')
        self.emit('    local left = {}')
        self.emit('    for i, c in ipairs(pending) do')
        self.emit('      local ok, r = pcall(connect, c[1], c[2])')
        self.emit('      if not (ok and r) then table.insert(left, c) end')
        self.emit('    end')
        self.emit('    LOG("connect round " .. round .. ": " .. (table.getn(pending) - table.getn(left)) .. " made, " .. table.getn(left) .. " pending")')
        self.emit('    if table.getn(left) == table.getn(pending) then pending = left; break end')
        self.emit('    pending = left')
        self.emit('  end')
        self.emit('  CONNS = {}')
        self.emit('  return pending')
        self.emit('end')
        self.emit('P = {}   -- node id / container path -> Connect path')
        self.emit('-- moves the TrajectoryJoint / HipJoint tags of a .mcarig onto the given joints')
        self.emit('function TAGRIG(path, traj, hip)')
        self.emit('  local fh = io.open(path, "rb"); if not fh then return false end')
        self.emit('  local s = fh:read("*a"); fh:close()')
        self.emit('  local nl = string.find(s, "\\r\\n", 1, true) and "\\r\\n" or "\\n"')
        self.emit('  s = string.gsub(s, "([ \\t]*)<StringArray name=\\"Tags\\" size=\\"%d+\\">(.-)</StringArray>\\r?\\n", function(ind, body)')
        self.emit('    local keep = {}')
        self.emit('    string.gsub(body, "<S>(.-)</S>", function(t) if t ~= "TrajectoryJoint" and t ~= "HipJoint" then table.insert(keep, t) end end)')
        self.emit('    if table.getn(keep) == 0 then return "" end')
        self.emit('    local out = ind .. "<StringArray name=\\"Tags\\" size=\\"" .. table.getn(keep) .. "\\">" .. nl')
        self.emit('    for i, t in ipairs(keep) do out = out .. "<S>" .. t .. "</S>" .. nl end')
        self.emit('    return out .. ind .. "</StringArray>" .. nl')
        self.emit('  end)')
        self.emit('  local tags = {}')
        self.emit('  tags[traj] = { "TrajectoryJoint" }')
        self.emit('  if hip == traj then table.insert(tags[traj], "HipJoint") else tags[hip] = { "HipJoint" } end')
        self.emit('  for joint, list in pairs(tags) do')
        self.emit('    local pat = "(\\n([ \\t]*)<Node name=\\"" .. string.gsub(joint, "(%W)", "%%%1") .. "\\" type=\\"sgTransformNode\\" module=\\"nmx\\">\\r?\\n)"')
        self.emit('    local n')
        self.emit('    s, n = string.gsub(s, pat, function(line, ind)')
        self.emit('      local out = line .. ind .. "\\t<StringArray name=\\"Tags\\" size=\\"" .. table.getn(list) .. "\\">" .. nl')
        self.emit('      for i, t in ipairs(list) do out = out .. "<S>" .. t .. "</S>" .. nl end')
        self.emit('      return out .. ind .. "\\t</StringArray>" .. nl')
        self.emit('    end, 1)')
        self.emit('    if n ~= 1 then LOG("FAIL  tagRigJoints: joint " .. joint .. " not found"); return false end')
        self.emit('  end')
        self.emit('  fh = io.open(path, "wb"); fh:write(s); fh:close()')
        self.emit('  LOG("rig tags: TrajectoryJoint=" .. traj .. " HipJoint=" .. hip)')
        self.emit('  return true')
        self.emit('end')
        self.emit()

    def setup(self):
        self.emit('-- 1. animation rigs (one per animation set) + project')
        for sn, (sj, sr) in zip(self.set_names, self.set_rigs):
            hip = traj = None
            if sr is not None:
                hip = sj.get(int(sr.findtext('hipIndex') or -1))
                traj = sj.get(int(sr.findtext('trajectoryIndex') or -1))
            self.emit('RIG = ROOT .. %s' % lua_str('\\' + sn + '.mcarig'))
            self.emit('local fh = io.open(RIG, "r")')
            self.emit('if fh then fh:close() else')
            if self.model:
                self.emit('  TRY("anim.createRig %s", function() return anim.createRig(ROOT .. %s, RIG, %s, %s) end)' % (
                    sn, lua_str('\\model_xmd\\' + os.path.basename(self.model)), lua_val(hip or ''), lua_val(traj or '')))
            self.emit('end')
            if hip and traj:
                # anim.createRig ignores its hip/trajectory arguments and tags the first joint for both, so retag the rig file
                self.emit('TRY("tagRigJoints %s", function() return TAGRIG(RIG, %s, %s) end)' % (sn, lua_str(traj), lua_str(hip)))
        self.emit('TRY("project.open", function() return project.open(ROOT .. "\\\\" .. NAME .. ".mcp") end)')
        self.emit('TRY("mcn.new", function() return mcn.new() end)')
        self.emit('SET = %s' % lua_str(self.set_name))
        self.emit('SETS = %s' % lua_val(self.set_names))
        self.emit('TRY("setSelectedAnimSet", function() return setSelectedAnimSet(SET) end)')
        self.emit('LOG("animsets=" .. TS(listAnimSets()) .. " rig=" .. TS(anim.getRigPath(SET)))')
        self.mirror_mappings()
        self.emit()

    def mirror_mappings(self):
        """Joint and event mirror mappings: anim.createRig makes a rig without them, so MirrorTransforms would
        mirror every joint in place instead of swapping left and right."""
        self.emit('-- mirror mappings (joints from each set .mrarig, events from the library)')
        self.emit('function ADDMIRRORS(set, kind, wanted)')
        self.emit('  local have = {}')
        self.emit('  local ok, cur = pcall(anim["listAnimSet" .. kind .. "MirrorMappings"], set)')
        self.emit('  if ok and type(cur) == "table" then for i, mp in ipairs(cur) do have[tostring(mp.first) .. "|" .. tostring(mp.second)] = true end end')
        self.emit('  local add = {}')
        self.emit('  for i, mp in ipairs(wanted) do if not have[tostring(mp.first) .. "|" .. tostring(mp.second)] then table.insert(add, mp) end end')
        self.emit('  if table.getn(add) == 0 then return true end')
        self.emit('  return anim["addAnimSet" .. kind .. "MirrorMappings"](set, add)')
        self.emit('end')
        for k, (sn, (sj, sr)) in enumerate(zip(self.set_names, self.set_rigs)):
            if sr is not None:
                pairs = [(sj.get(int(mp.get('first'))), sj.get(int(mp.get('second')))) for mp in sr.findall('JointMirrorMapping')]
                pairs = [{'first': a, 'second': b} for a, b in pairs if a and b]
                if pairs:
                    self.call('joint mirror mappings %s' % sn, 'ADDMIRRORS(%s, "Joint", %s)' % (lua_str(sn), lua_val(pairs)))
            if k < len(self.lib.event_mirrors) and self.lib.event_mirrors[k]:
                ev = [{'first': a, 'second': b} for a, b in self.lib.event_mirrors[k]]
                self.call('event mirror mappings %s' % sn, 'ADDMIRRORS(%s, "EventUserdata", %s)' % (lua_str(sn), lua_val(ev)))
            if k < len(self.lib.track_mirrors) and self.lib.track_mirrors[k]:
                tr = [{'first': a, 'second': b} for a, b in self.lib.track_mirrors[k]]
                self.call('event track mirror mappings %s' % sn, 'ADDMIRRORS(%s, "EventTrack", %s)' % (lua_str(sn), lua_val(tr)))

    def requests(self):
        self.emit('-- 2. requests (runtime ids preserved)')
        for m in sorted(self.msgs, key=lambda m: int(m['messageID'])):
            if m.get('messageTypeID') != '10':
                self.unsupported.append('message %s type %s' % (m['name'], m.get('messageTypeID'))); continue
            self.call('createRequest %s' % m['name'], 'create("Request", %s, %d)' % (lua_str(m['name']), int(m['messageID'])),
                      'P[%s]' % lua_str('#msg%s' % m['messageID']))
        self.emit()

    # ---- control parameters
    def xml_default(self, n):
        if n.type in (21, 22): return [n.get('DefaultValue_%d' % i, 0.0) for i in range(3 if n.type == 21 else 4)]
        if n.type in (24, 25): return [n.get('DefaultInt', n.get('DefaultUInt', 0))]
        if n.type == 23: return [1.0 if n.get('DefaultFlag', n.get('DefaultBool', False)) else 0.0]
        return [n.get('DefaultValue_0', 0.0)]

    def cp_settings(self, n, key, force=False):
        nm = n.name.split('|')[-1]
        cfg = self.cp_config.get(nm, {})
        rng = {k: cfg[k] for k in ('min', 'max') if cfg.get(k) is not None}
        # setRange refuses vector CPs, so their ranges are written into the .mcn by --inject (inject_cp_ranges)
        if rng and n.type in (20, 24, 25):   # bool CPs have no range
            self.call('setRange %s' % nm, 'setRange(%s, %s)' % (key, lua_val({k: float(v) for k, v in rng.items()})))
        vals = self.xml_default(n)   # defaults always come from the export; a 'default' in the CP file is ignored
        if force or any(v != 0 for v in vals):
            if n.type == 23:
                self.call('setDefaultValue %s' % nm, 'setDefaultValue(%s, %s)' % (key, lua_val(bool(vals[0]))))
            else:
                self.call('setDefaultValue %s' % nm, 'setDefaultValue(%s, %s)' % (key, ', '.join(lua_val(float(v)) for v in vals)))
        if nm in self.cp_config: self.cp_used.add(nm)

    def control_params(self):
        self.emit('-- 3. control parameters')
        for n in self.cps:
            nm = n.name.split('|')[-1]
            self.call('create CP %s' % nm, 'create("ControlParameter", %s, %s)' % (lua_str(CP_TYPES[n.type]), lua_str(nm)), self.ref(n.id))
            self.cp_settings(n, self.ref(n.id))
        self.emit()

    # ---- graph
    def graph(self):
        nodes = self.nodes
        self.emit('-- 4. graph: state machines, BlendTree states, nested blend trees, nodes')
        items = [(p, kind) for p, kind in self.containers.items() if p and kind in ('state', 'nbt')]
        items += [(p, 'sm') for p in self.sms]
        items += [(n.id, 'node') for n in self.gnodes]
        def path_of(it): return nodes[it[0]].name if it[1] == 'node' else it[0]
        items.sort(key=lambda it: (path_of(it).count('|'), path_of(it), str(it[0])))
        for it, kind in items:
            path = path_of((it, kind))
            parent, leaf = parent_path(path), path.split('|')[-1]
            pexpr = self.ref(parent) if parent else '""'
            if kind == 'sm':
                self.call('create StateMachine %s' % path, 'create("StateMachine", %s, %s)' % (pexpr, lua_str(leaf)), self.ref(path))
                self.emit('P[%d] = %s' % (self.sms[path].id, self.ref(path)))
            elif kind in ('state', 'nbt'):
                self.call('create BlendTree %s' % path, 'create("BlendTree", %s, %s)' % (pexpr, lua_str(leaf)), self.ref(path))
            else:
                mtype = self.spec[it][0]
                self.call('create %s #%d %s' % (mtype, it, path), 'create(%s, %s, %s)' % (lua_str(mtype), pexpr, lua_str(leaf)), self.ref(it))
        self.emit()
        self.emit('-- 5. node attributes')
        for n in self.gnodes:
            mtype, attrs, inputs = self.spec[n.id]
            for a, v, per in attrs:
                if a in LATE_ATTRS: continue
                self.set_attr(self.ref(n.id), a, v, per, '%s #%d .%s' % (mtype, n.id, a))
            for k, ak in sorted(self.spec_sets.get(n.id, {}).items()):
                for a, v, per in ak:
                    if a in LATE_ATTRS: continue
                    self.set_attr(self.ref(n.id), a, v, per, '%s #%d .%s set %d' % (mtype, n.id, a, k), 'SETS[%d]' % k)
        self.emit()

    def late_attributes(self):
        self.emit('-- weights: BlendN/Switch rebuild them whenever a pin is connected, so they go last')
        for n in self.gnodes:
            mtype, attrs, inputs = self.spec[n.id]
            for a, v, per in attrs:
                if a in LATE_ATTRS: self.set_attr(self.ref(n.id), a, v, per, '%s #%d .%s' % (mtype, n.id, a))
            for k, ak in sorted(self.spec_sets.get(n.id, {}).items()):
                for a, v, per in ak:
                    if a in LATE_ATTRS: self.set_attr(self.ref(n.id), a, v, per, '%s #%d .%s set %d' % (mtype, n.id, a, k), 'SETS[%d]' % k)
        self.emit()

    def value_expr(self, v):
        if isinstance(v, tuple) and v and v[0] == '#request': return self.ref('#msg%d' % v[1])
        if isinstance(v, tuple) and v and v[0] == '#cp': return self.ref(v[1])
        if isinstance(v, tuple) and v and v[0] == '#path': return lua_str(v[1])
        if isinstance(v, tuple) and v and v[0] == '#node':
            st = self.state_of_root(v[1])
            return self.ref(st) if st else 'nil'
        return lua_val(v)

    def set_attr(self, me, attr, val, per_set=False, desc=None, set_var='SET'):
        self.call(desc or 'setAttribute .%s' % attr, 'setAttribute(%s .. ".%s", %s%s)' % (me, attr, self.value_expr(val), (', ' + set_var) if per_set else ''))

    def state_of_root(self, nid):
        """State path whose runtime node is nid (InSubState / DestinationSubState references)."""
        n = self.nodes.get(nid)
        if n is None: return None
        comps = n.name.split('|')
        for k in range(len(comps), 0, -1):
            p = '|'.join(comps[:k])
            if self.is_state(p) and (p in self.sms or self.croot.get(p) == nid): return p
        return None

    # ---- connections
    def connections(self):
        """Queue every connection: container results, node inputs, pass-down routing."""
        nodes = self.nodes
        self.pins = {}          # (container, key) -> pin expr
        self.pin_defs = []      # (container, pin name) for states (injected) / Lua created
        self.edges1, self.edges2 = [], []   # (src expr, dst expr, depth)
        done = set()

        def out_of(item):   # output pin expression of a node id or container path
            return '%s .. ".Result"' % self.ref(item)

        def add(src, dst, depth, stage2=False):
            k = (src, dst)
            if k in done: return
            done.add(k)
            (self.edges2 if stage2 else self.edges1).append((src, dst, depth))

        # container roots -> container Result
        for p, rid in self.croot.items():
            if rid not in nodes: continue
            item, direct = self.item_in(p, rid)
            src = out_of(rid if direct else item)
            dst = '%s .. ".Result"' % self.ref(p) if p else '".Result"'
            add(src, dst, p.count('|') + 1 if p else 0)

        def pin_on(container, key, label):
            k = (container, key)
            if k not in self.pins:
                name = re.sub(r'\W', '_', 'In_%s' % label)[:48]
                used = {pn for (c, _), pn in self.pins.items() if c == container}
                base, i = name, 1
                while name in used: name = '%s_%d' % (base, i); i += 1
                self.pins[k] = name
                self.pin_defs.append((container, name))
            return self.pins[k]

        def pin_expr(container, name): return '%s .. ".%s"' % (self.ref(container), name)

        for n in self.gnodes:
            mtype, attrs, inputs = self.spec[n.id]
            gc = parent_path(n.name)
            for field, pin in inputs:
                sid = n.get(field)
                s = nodes.get(sid)
                if s is None: continue
                dst = '%s .. ".%s"' % (self.ref(n.id), pin)
                depth = gc.count('|') + 1 if gc else 0
                if s.type in CP_TYPES:
                    add('%s .. ".Result"' % self.ref(s.id), dst, depth); continue
                gs = parent_path(s.name)
                # lowest common graph
                L = gs
                while not (gc == L or gc.startswith(L + '|') or L == ''): L = parent_path(L)
                if L == gs: src_item, src_direct = s.id, True
                else: src_item, src_direct = self.item_in(L, s.id)
                src = out_of(src_item)
                if gc == L:
                    add(src, dst, depth); continue
                chain, p = [], gc
                while p != L: chain.insert(0, p); p = parent_path(p)
                key = ('src', src_item)
                label = (nodes[src_item].name if isinstance(src_item, int) else src_item).split('|')[-1]
                prev = src
                for i, c in enumerate(chain):
                    pn = pin_on(c, key, label)
                    stage2 = self.is_state(c) or (i > 0 and self.is_state(chain[i - 1]))
                    add(prev, pin_expr(c, pn), c.count('|') + 1, stage2)
                    prev = pin_expr(c, pn)
                add(prev, dst, depth, self.is_state(chain[-1]))

    def emit_pins_and_edges(self):
        self.emit('-- 6. pass-down pins (nested blend trees / state machines; state pins are injected)')
        for c, name in self.pin_defs:
            if not self.is_state(c):
                self.call('pin %s.%s' % (c, name), 'create("PassDownPin", %s, %s)' % (self.ref(c), lua_str(name)))
        self.emit()
        self.emit('-- 7. connections (deepest graphs first)')
        for src, dst, depth in sorted(self.edges1, key=lambda e: -e[2]):
            self.emit('CONN(%s, %s)' % (src, dst))
        self.emit('local left = RUNCONNS()')
        self.emit('PENDING = left')
        self.emit()

    # ---- transitions
    def plan_transitions(self):
        nodes = self.nodes
        self.self_transits, self.deferred = [], []
        cond_src = {}
        for h in nodes.values():
            for t, idx in h.condsets: cond_src.setdefault(t, []).append((h, idx, False))
            for t, idx in h.common_condsets: cond_src.setdefault(t, []).append((h, idx, True))
        self.active_states, self.tplan = {}, []
        for t in self.transits:
            sm = nodes[t.parent]
            tname = t.name.split('|')[-1]
            dst_state = self.state_in(sm.name, t.get('DestNodeID'))
            src = t.get('SourceNodeID')
            holders = cond_src.get(t.id, [])
            if src is not None:
                self.tplan.append((t, self.state_in(sm.name, src), dst_state, holders, False)); continue
            common = any(c for _, _, c in holders)
            as_name = tname.split('_')[0] if tname.startswith('ActiveState') else ('ActiveState' if common else 'ActiveStateSubset%d' % t.id)
            states = sorted({(h.id if h.type in TRANSIT_TYPES else self.state_in(sm.name, h.id)) for h, _, c in holders if not c} - {None}, key=str)
            key = (sm.name, as_name)
            prev = self.active_states.get(key)
            if prev and (prev[0] != common or (not common and prev[1] != states)):
                as_name = '%s_%d' % (as_name, t.id); key = (sm.name, as_name)
            self.active_states[key] = (common, states)
            self.tplan.append((t, sm.name + '|' + as_name, dst_state, holders, True))

    def transitions(self, active):
        self.emit('-- 8. transitions and conditions (%s)' % ('from ActiveStates' if active else 'state to state'))
        for t, src, dst_state, holders, from_as in self.tplan:
            if from_as != active: continue
            if src is None or dst_state is None:
                self.unsupported.append('transition #%d %s: source/dest state not found' % (t.id, t.name)); continue
            tkey = self.ref(t.id)
            tname = t.name.split('|')[-1]
            mtype = TRANSIT_TYPES[t.type]
            if src == dst_state:
                # create() refuses source == destination: aim at a sibling state, --inject retargets it to self
                sib = next((p for p, k in sorted(self.containers.items()) if parent_path(p) == parent_path(src) and p != src
                            and (k == 'state' or p in self.sms)), None)
                if sib is None:
                    self.unsupported.append('transition to self #%d %s: no sibling state' % (t.id, t.name)); continue
                self.self_transits.append(t.id)
                dst_state = sib
            self.call('create %s #%d %s' % (mtype, t.id, tname), 'create(%s, %s, %s, %s)' % (
                lua_str(mtype), self.ref(src), self.ref(dst_state), lua_str(tname)), tkey)
            self.attrs_transit(t, tkey)
            if holders:
                h, idx, common = holders[0]
                table = h.common_conditions if common else h.conditions
                for k, ci in enumerate(idx):
                    self.condition(table[ci], tkey, '#t%d.c%d' % (t.id, k))
        self.emit()

    def attrs_transit(self, t, me):
        g = t.get
        flags = ['AdditiveBlendAttitude', 'AdditiveBlendPosition', 'SphericallyInterpolateTrajectoryPosition',
                 'DeadblendBreakoutToSource', 'BreakoutTransit', 'UseDeadReckoningWhenDeadBlending', 'BlendToDestinationPhysicsBones']
        if t.type == 402:
            flags += ['UseDestinationStartFraction', 'UseDestinationStartSyncEventIndex', 'UseDestinationStartSyncEventFraction',
                      'FreezeSource', 'FreezeDest']
            self.set_attr(me, 'DurationInTime', float(g('DurationInTime', 0.0)))
            self.set_attr(me, 'DestinationStartFraction', float(g('DestinationStartFraction', 0.0)))
            ev = float(g('DestinationStartSyncEvent', 0.0))
            # DS2's Transit.lua keeps the whole value (index + fraction) in DestinationStartSyncEvent; the fraction
            # attribute still feeds the fraction-only mode
            self.set_attr(me, 'DestinationStartSyncEvent', ev)
            self.set_attr(me, 'DestinationStartSyncEventFraction', float(ev - math.floor(ev)) if g('UseDestinationStartSyncEventIndex') else ev)
        else:
            self.set_attr(me, 'DurationInEvents', float(g('DurationInEvents', 0.0)))
            for a in ('DestEventSequenceOffset', 'DestStartEventIndex'):
                if g(a) is not None: self.set_attr(me, a, int(g(a)))
            flags += ['UsingDestStartEventIndex']
            nodespecs.dur_events(t, extra := [])
            for a, v, per in extra: self.set_attr(me, a, v)
        for a in flags:
            if g(a) is not None: self.set_attr(me, a, bool(g(a)))
        dts = int(g('DeltaTrajSource', 3))
        self.set_attr(me, 'DeltaTrajSource', dts if dts in (1, 2) else 3)
        if g('isReversibleTransit'):
            self.set_attr(me, 'ReversibleTransit', True)
            if self.nodes.get(g('RuntimeNodeID')) is not None:
                self.set_attr(me, 'ReverseControlParameter', ('#cp', g('RuntimeNodeID')))
        cnt = int(g('DestinationSubStateCount', 0) or 0)
        if cnt > 0:
            st = self.state_of_root(g('DestinationSubStateID_%d' % (cnt - 1)))
            if not st:
                self.unsupported.append('transition #%d %s: destination sub state %r is not a state root' % (
                    t.id, t.name, g('DestinationSubStateID_%d' % (cnt - 1))))
            else:
                expr = ('DestinationSubState #%d' % t.id, 'setAttribute(%s .. ".DestinationSubState", %s)' % (me, self.ref(st)))
                if t.id in self.self_transits: self.deferred.append(expr)   # only valid once retargeted to self
                else: self.call(*expr)

    def condition(self, c, tkey, label):
        mtype, attrs = nodespecs.cond_spec(self, c)
        if mtype is None:
            self.unsupported.append('condition type %d on %s' % (c.type, label)); return
        ckey = 'P[%s]' % lua_str(label)
        self.call('create %s %s' % (mtype, label), 'create(%s, %s)' % (lua_str(mtype), tkey), ckey)
        for a, v, per in attrs: self.set_attr(ckey, a, v, per)

    def default_states(self):
        self.emit('-- 9. default states')
        for p, sm in self.sms.items():
            st = self.state_in(p, sm.get('DefaultNodeID'))
            if st: self.call('setDefaultState %s' % p, 'setDefaultState(%s, %s)' % (self.ref(p), self.ref(st)))
        self.emit()

    def auto_layout(self):
        """Positions computed in Python (layout.py) and emitted as plain calls: states placed to minimise
        transition crossings, blend trees flowing right to left into the Output node, ControlParameters
        node before the last (leftmost) node.  Connect's Lua aborts long loops ("Too many instructions")."""
        import layout
        nodes = self.nodes
        def nsize(item):
            if isinstance(item, int):
                mtype, attrs, inputs = self.spec.get(item, ('', [], []))
                npins = 13 if mtype in ('BlendN', 'Switch', 'Sequence') else len(inputs) + 1   # dynamic pin lists show every slot
                return layout.node_size(nodes[item].name.split('|')[-1], npins)
            return layout.node_size(item.split('|')[-1], 2)
        self.emit('-- layout (computed by layout.py)')
        children = {}
        for n in self.gnodes: children.setdefault(parent_path(n.name), []).append(n.id)
        for p, kind in self.containers.items():
            if p and kind in ('nbt', 'sm') and not self.is_state(p): children.setdefault(parent_path(p), []).append(p)
        cp_readers = set()
        for n in self.gnodes:
            for field, pin in self.spec[n.id][2]:
                s = nodes.get(n.get(field))
                if s is not None and s.type in CP_TYPES: cp_readers.add(n.id)
        for g, kind in self.containers.items():
            if kind not in ('root', 'state', 'nbt'): continue
            items = children.get(g, [])
            if not items: continue
            itemset = set(items)
            def item_of(nid):
                n = nodes[nid]
                if g and not n.name.startswith(g + '|'): return None
                path, direct = self.item_in(g, nid)
                it = nid if direct and nodes[nid].type != SM_TYPE else path
                return it if it in itemset else None
            edges = []
            for n in self.gnodes:
                if g and not n.name.startswith(g + '|'): continue
                d = item_of(n.id)
                if d is None: continue
                order = self.manifest.get(self.spec[n.id][0], {}).get('pinOrder', [])
                for k, (field, pin) in enumerate(self.spec[n.id][2]):
                    s = nodes.get(n.get(field))
                    if s is None or s.type in CP_TYPES or s.id not in self.spec and s.type != SM_TYPE: continue
                    si = item_of(s.id)
                    slot = order.index(pin) if pin in order else 100 + k   # manifest pin order: input 1 above input 2
                    if si is not None and si != d: edges.append((si, d, slot))
            pos, out, cp = layout.blend_tree(items, edges, nsize)
            gref = self.ref(g) if g else '""'
            self.emit('pcall(setOutputNodePosition, %s, %d, %d)' % (gref, out[0], out[1]))
            if any(i in cp_readers for i in items if isinstance(i, int)):
                self.emit('pcall(setControlParametersNodePosition, %s, %d, %d)' % (gref, cp[0], cp[1]))
            for it, (x, y) in pos.items():
                self.emit('pcall(setNodePosition, %s, %d, %d)' % (self.ref(it), x, y))
        for sm in self.sms:
            states = [p for p, k in self.containers.items() if parent_path(p) == sm and (k == 'state' or p in self.sms)]
            actives = [sm + '|' + a for (s, a) in self.active_states if s == sm]
            trans = [(src, dst) for t, src, dst, h, fa in self.tplan if src and dst and parent_path(dst) == sm]
            pos = layout.state_machine(states, actives, trans, nsize)
            for it, (x, y) in pos.items():
                self.emit('pcall(setNodePosition, %s, %d, %d)' % (self.ref(it), x, y))
        self.emit()

    def layout(self):
        self.emit('-- 10. layout (synthetic: original editor positions are not in the export)')
        children = {}
        for p, kind in self.containers.items():
            if p: children.setdefault(parent_path(p), []).append(p)
        for n in self.gnodes: children.setdefault(parent_path(n.name), []).append(n.id)
        for g, items in children.items():
            cols = max(1, int(math.ceil(math.sqrt(len(items)))))
            for k, it in enumerate(sorted(items, key=str)):
                self.emit('pcall(setNodePosition, %s, %d, %d)' % (self.ref(it), 300 * (k % cols), 140 * (k // cols)))
        self.emit()

    def save_paths(self):
        """Stage 2 needs the paths Connect really gave (duplicate names get suffixes)."""
        self.emit('local pf = io.open(BUILD .. "\\\\" .. NAME .. "_paths.lua", "w")')
        self.emit('for k, v in pairs(P) do if type(k) == "number" then pf:write(string.format("P[%d] = %q\\n", k, v)) else pf:write(string.format("P[%q] = %q\\n", k, v)) end end')
        self.emit('for i, c in ipairs(PENDING or {}) do pf:write(string.format("CONN(%q, %q)\\n", c[1], c[2])) end')
        self.emit('pf:close()')

    def epilogue(self, export=True):
        self.emit('TRY("mcn.saveAs", function() return mcn.saveAs(ROOT .. "\\\\" .. NAME .. ".mcn") end)')
        if export:
            self.emit('pcall(function() app.createDirectory(BUILD .. "\\\\roundtrip") end)')
            self.emit('local ok, res, ids, errors, warnings = pcall(mcn.export, BUILD .. "\\\\roundtrip\\\\" .. NAME .. ".xml")')
            self.emit('LOG("export ok=" .. tostring(ok) .. " result=" .. TS(res))')
            self.emit('if type(errors) == "table" then for i, v in ipairs(errors) do LOG("EXPORT ERROR " .. TS(v.name) .. " : " .. TS(v.message)) end end')
        self.emit('LOG("DONE failures=" .. NFAIL)')
        self.emit('LOGF:close()')

    def anim_set_options(self):
        for k, (sn, entries) in enumerate(self.lib.sets):
            opts = {e['options'] for e in entries.values()}
            if len(opts) == 1 and list(opts)[0]:
                self.call('anim.setAnimSetOptions %s' % sn, 'anim.setAnimSetOptions(%s, %s)' % (lua_str(sn), lua_str(list(opts)[0])))

    def write_lua(self, suffix):
        lua = os.path.join(self.build_dir, self.name + suffix)
        with open(lua, 'w', newline='\r\n') as f: f.write('\n'.join(self.lines) + '\n')
        return lua

    def report_missing_cp_config(self):
        """<name>_cp_config_missing.log: control parameters with no entry in the CP settings file."""
        missing = [n for n in self.cps if n.name.split('|')[-1] not in self.cp_config]
        path = os.path.join(self.build_dir, self.name + '_cp_config_missing.log')
        with open(path, 'w', encoding='utf-8') as f:
            f.write('# %d of %d control parameters have no entry in the CP settings file; export defaults used\n' % (len(missing), len(self.cps)))
            for n in missing:
                d = [float(v) for v in self.xml_default(n)]
                f.write('%s\t%s\tdefault=%s\n' % (n.name.split('|')[-1], CP_TYPES[n.type], d if len(d) > 1 else d[0]))
        self.missing_cp = len(missing)
        return path

    def run(self):
        self.analyse(); self.plan_transitions(); self.connections()
        self.report_missing_cp_config()
        self.lines = []
        self.prologue('_rebuild.log'); self.setup(); self.anim_set_options(); self.requests(); self.control_params()
        self.graph(); self.emit_pins_and_edges(); self.transitions(False); self.default_states()
        if not self.needs_stage2(): self.late_attributes(); self.auto_layout()
        self.save_paths(); self.epilogue(export=not self.needs_stage2())
        self.write_project()
        out = [self.write_lua('_rebuild.lua')]
        if self.needs_stage2():
            self.lines = []
            self.prologue('_stage2.log')
            self.emit('TRY("mcn.open", function() return mcn.open(ROOT .. "\\\\" .. NAME .. ".mcn") end)')
            self.emit('SET = %s' % lua_str(self.set_name))
            self.emit('SETS = %s' % lua_val(self.set_names))
            self.emit('TRY("setSelectedAnimSet", function() return setSelectedAnimSet(SET) end)')
            self.emit('dofile(BUILD .. "\\\\" .. NAME .. "_paths.lua")   -- paths + connections that failed in stage 1')
            for (smname, as_name) in self.active_states:
                self.emit('P[%s] = %s .. "|%s"' % (lua_str(smname + '|' + as_name), self.ref(smname), as_name))
            self.emit('-- connections through injected state pass-down pins')
            for src, dst, depth in sorted(self.edges2, key=lambda e: -e[2]):
                self.emit('CONN(%s, %s)' % (src, dst))
            self.emit('local left = RUNCONNS()')
            self.emit('for i, c in ipairs(left) do NFAIL = NFAIL + 1; LOG("FAIL  connect " .. c[1] .. " -> " .. c[2]) end')
            self.transitions(True)
            for desc, expr in self.deferred: self.call(desc, expr)
            self.late_attributes()
            self.auto_layout()
            self.emit('PENDING = {}')
            self.save_paths()
            self.epilogue()
            out.append(self.write_lua('_stage2.lua'))
        if self.needs_stage3():
            # ActiveStates listing transitions that start at an ActiveState: those only exist after stage 2
            self.lines = []
            self.prologue('_stage3.log')
            self.emit('TRY("mcn.open", function() return mcn.open(ROOT .. "\\\\" .. NAME .. ".mcn") end)')
            self.epilogue()
            out.append(self.write_lua('_stage3.lua'))
        return out

    def late_members(self):
        """ActiveState members that are transitions from an ActiveState (created in stage 2)."""
        late = set(t.id for t, src, dst, h, fa in self.tplan if fa)
        return {k: [st for st in states if st in late] for k, (allst, states) in self.active_states.items() if not allst}

    def needs_stage3(self):
        return self.needs_stage2() and any(self.late_members().values())

    def needs_stage2(self):
        return bool(self.active_states) or bool(self.edges2)

    # ------------------------------------------------------------------ .mcn injection
    def _mcn(self, mcn_path):
        tree = ET.parse(mcn_path)
        return tree, tree.getroot().find('MorphemeDB/Networks/Network')

    def _save(self, tree, mcn_path):
        ET.indent(tree, '\t')
        with open(mcn_path, 'wb') as f:
            f.write(b'<?xml version="1.0" encoding="UTF-8"?>\r\n')
            f.write(ET.tostring(tree.getroot(), encoding='utf-8').replace(b'\n', b'\r\n'))

    def locate(self, net, path, P):
        """(element, graph element, pointer) of the object at a Connect path (as Connect named it)."""
        def graph_of(elem, ptr):
            g = list(elem.find('GraphEntry'))[0]
            return g, '%s.GraphEntry.%s' % (ptr, g.get('name'))
        g, gptr = graph_of(net, 'MorphemeDB.Networks.Network')   # graph element + pointer of the current level
        elem = net
        if not path: return elem, g, gptr
        for comp in path.split('|'):
            for cont in ('BlendTreeNodes', 'StateMachineNodes'):
                c = g.find(cont)
                hit = c.find('*[@name="%s"]' % comp) if c is not None else None
                if hit is not None: break
            else:
                raise KeyError('%s: %s not found' % (path, comp))
            elem = hit
            ge = hit.find('GraphEntry')
            if ge is not None and len(list(ge)):
                g, gptr = graph_of(hit, '%s.%s.%s' % (gptr, cont, comp))
        return elem, g, gptr

    def real_paths(self):
        paths = {}
        pf = os.path.join(self.build_dir, self.name + '_paths.lua')
        if os.path.exists(pf):
            for line in open(pf, encoding='latin1'):
                m = re.match(r'P\[(?:(\d+)|"(.*)")\] = "(.*)"$', line.strip())
                if m: paths[int(m.group(1)) if m.group(1) else m.group(2).replace('\\\\', '\\')] = m.group(3)
        return paths

    def inject(self, mcn_path):
        """ActiveStates, pass-down pins on states and CP groups into the stage-1 .mcn."""
        tree, net = self._mcn(mcn_path)
        P = self.real_paths()
        rp = lambda key: P.get(key, key if isinstance(key, str) else None)
        added = 0
        for (smname, as_name), (allstates, states) in sorted(self.active_states.items()):
            sm_el, g, gptr = self.locate(net, rp(smname), P)
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
                    if rp(st) is None:
                        self.unsupported.append('ActiveState %s member %r was not created in stage 1' % (as_name, st)); continue
                    leaf = rp(st).split('|')[-1]
                    cont = 'TransitionEdges' if g.find('TransitionEdges/*[@name="%s"]' % leaf) is not None else 'StateMachineNodes'
                    ET.SubElement(v, 'elem', type='pointer').text = '%s.%s.%s' % (gptr, cont, leaf)
            for tag, val in (('XPosition', -400.0), ('YPosition', 90.0 * added), ('Width', 210.0), ('Height', 50.3984375)):
                ET.SubElement(n, tag, type='float').text = repr(val)
            ET.SubElement(n, 'ManifestVersion', type='int').text = '1'
            ET.SubElement(n, 'NodeType', type='string').text = 'ActiveState'
            added += 1
        pins = 0
        for c, name in self.pin_defs:
            if not self.is_state(c): continue
            el, g, ptr = self.locate(net, rp(c), P)
            pc = el.find('Pins')
            if pc is None: pc = ET.SubElement(el, 'Pins', type='nodeContainer')
            if pc.find('*[@name="%s"]' % name) is not None: continue
            pd = ET.Element('PassDownPin', name=name, type='node')
            ET.SubElement(pd, 'Input', type='bool').text = '1'
            ET.SubElement(pd, 'Reference', type='bool').text = '1'
            ET.SubElement(pd, 'Multiplicity', type='enum').text = 'OneToMany'
            pc.insert(0, pd)
            pins += 1
        # transitions to self were created towards a sibling: point them back at their source
        for tid in self.self_transits:
            tp = P.get(tid)
            if not tp: self.unsupported.append('self transition #%d not created' % tid); continue
            sm_el, g, gptr = self.locate(net, parent_path(tp), P)
            edge = g.find('TransitionEdges/TransitionEdge[@name="%s"]' % tp.split('|')[-1])
            if edge is None: self.unsupported.append('self transition %s not found in .mcn' % tp); continue
            edge.find('To').text = edge.find('From').text
        groups = self.inject_cp_groups(net)
        self.inject_cp_ranges(net)
        self._save(tree, mcn_path)
        return added, pins, groups

    def inject_late(self, mcn_path):
        """Add the stage-2 transitions to the ActiveStates that list them (after stage 2 saved the .mcn)."""
        tree, net = self._mcn(mcn_path)
        P = self.real_paths()
        added = 0
        for (smname, as_name), members in sorted(self.late_members().items()):
            if not members: continue
            sm_el, g, gptr = self.locate(net, P.get(smname, smname), P)
            n = g.find('StateMachineNodes/*[@name="%s"]' % as_name)
            ra = n.find('Attributes/RefArrayAttribute[@name="States"]') if n is not None else None
            if ra is None: self.unsupported.append('ActiveState %s not found after stage 2' % as_name); continue
            v = ra.find('Value')
            if v is None: v = ET.SubElement(ra, 'Value', type='attributeArray', size='0', elemType='pointer')
            have = {e.text for e in v}
            for tid in members:
                if tid not in P: self.unsupported.append('ActiveState %s member transition #%d not created' % (as_name, tid)); continue
                ref = '%s.TransitionEdges.%s' % (gptr, P[tid].split('|')[-1])
                if ref in have: continue
                ET.SubElement(v, 'elem', type='pointer').text = ref
                added += 1
            v.set('size', str(len(v)))
        self._save(tree, mcn_path)
        return added

    def inject_cp_groups(self, net):
        wanted = {}
        for n in self.cps:
            g = (self.cp_config.get(n.name.split('|')[-1]) or {}).get('group')
            if g: wanted.setdefault(g, []).append(n.name.split('|')[-1])
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
        for grp in container.findall('ControlParameterGroup'):
            ga = grp.find('ControlParameterArray')
            if ga is None: continue
            for e in list(ga):
                if e.text and e.text[len(ptr):] in regrouped: ga.remove(e)
            ga.set('size', str(len(ga)))
        for gname, cps in wanted.items():
            grp = container.find('ControlParameterGroup[@name="%s"]' % gname)
            if grp is None: grp = ET.SubElement(container, 'ControlParameterGroup', name=gname, type='node')
            ga = grp.find('ControlParameterArray')
            if ga is None: ga = ET.SubElement(grp, 'ControlParameterArray', type='attributeArray', elemType='pointer')
            for cp in cps:
                if cp not in existing:
                    self.unsupported.append('CP group %s: %s is not in the .mcn' % (gname, cp)); continue
                ET.SubElement(ga, 'elem', type='pointer').text = ptr + cp
            ga.set('size', str(len(ga)))
        for grp in container.findall('ControlParameterGroup'):
            ga = grp.find('ControlParameterArray')
            if ga is None or len(ga) == 0: container.remove(grp)
        return len(wanted)

    def inject_groups_only(self, mcn_path):
        tree, net = self._mcn(mcn_path)
        n = self.inject_cp_groups(net)
        if n or self.inject_cp_ranges(net): self._save(tree, mcn_path)
        return n

    def inject_cp_ranges(self, net):
        """Min / Max of vector CPs, stored like a float CP's: <Min type="float">x</Min> after DataPinEntry."""
        arr = net.find('ControlParametersNode/ControlParameterArray')
        if arr is None: return 0
        els = {c.get('name'): c for c in arr.findall('ControlParameter')}
        done = 0
        for n in self.cps:
            if n.type not in VECTOR_CP_TYPES: continue
            nm = n.name.split('|')[-1]
            cfg = self.cp_config.get(nm, {})
            el = els.get(nm)
            if el is None or all(cfg.get(k) is None for k in ('min', 'max')): continue
            pos = list(el).index(el.find('DataPinEntry')) + 1 if el.find('DataPinEntry') is not None else 0
            for tag, key in (('Min', 'min'), ('Max', 'max')):
                old = el.find(tag)
                if old is not None: el.remove(old)
                if cfg.get(key) is None: continue
                e = ET.Element(tag, type='float')
                e.text = repr(float(cfg[key])).rstrip('0').rstrip('.') if float(cfg[key]) != int(float(cfg[key])) else str(int(float(cfg[key])))
                el.insert(pos, e); pos += 1
            done += 1
        return done

    def cp_only(self):
        self.report_missing_cp_config()
        self.lines = []
        self.prologue('_cparams.log')
        self.emit('TRY("mcn.open", function() return mcn.open(ROOT .. "\\\\" .. NAME .. ".mcn") end)')
        for n in self.cps:
            if n.name.split('|')[-1] in self.cp_config:
                self.cp_settings(n, lua_str(n.name), force=True)
        self.emit('TRY("mcn.save", function() return mcn.save() end)')
        self.emit('LOG("DONE failures=" .. NFAIL)')
        self.emit('LOGF:close()')
        return self.write_lua('_cparams.lua')

    def write_cp_template(self, path):
        data = json.load(open(path, encoding='utf-8')) if os.path.exists(path) else {}
        for n in self.cps:
            nm = n.name.split('|')[-1]
            if nm in data:
                data[nm].setdefault('group', None); continue
            data[nm] = {'type': CP_TYPES[n.type], 'group': None, 'min': None, 'max': None}
        with open(path, 'w', encoding='utf-8') as f: json.dump(data, f, indent=2)
        return len(data)

    def write_project(self):
        sets = ''.join(f'''			<Node name="{sn}" type="AnimationSet" module="mcc">
				<String name="AnimationRig" val="$(RootDir)\\{sn}.mcarig"/>
				<String name="Format" val="nsa"/>
			</Node>
''' for sn in self.set_names)
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
{sets}		</Node>
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


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('xml')
    ap.add_argument('--library'); ap.add_argument('--rig'); ap.add_argument('--model'); ap.add_argument('--out-dir')
    ap.add_argument('--inject-late', action='store_true', help='add stage-2 transitions to the ActiveStates of the stage-2 .mcn')
    ap.add_argument('--inject', action='store_true', help='add ActiveStates, state pass-down pins and CP groups to the stage-1 .mcn')
    ap.add_argument('--cp-config', default=DEFAULT_CP_CONFIG,
                    help='JSON of control parameter settings by name: {"Name": {"group": g, "min": x, "max": y}} (defaults come from the export)')
    ap.add_argument('--cp-template', action='store_true', help='add the CPs of this network to --cp-config, then run as usual')
    ap.add_argument('--cp-only', action='store_true', help='write <name>_cparams.lua (and CP groups) for the existing .mcn')
    a = ap.parse_args()
    cp_config = {}
    if a.cp_config and os.path.exists(a.cp_config) and not a.cp_template:
        cp_config = {k: v for k, v in json.load(open(a.cp_config, encoding='utf-8')).items() if not k.startswith('_')}
    base = os.path.dirname(os.path.abspath(a.xml))
    name = os.path.splitext(os.path.basename(a.xml))[0]
    lib = Library(a.library or os.path.join(base, name + '_Library.xml'))
    rig = a.rig or next((os.path.join(base, f) for f in os.listdir(base) if f.endswith('.mrarig')), None)
    joints, rig_root = load_rig_joints(rig)
    # the character model lives in <project>\model_xmd\ (moved there if it sits next to the XML)
    model = a.model
    if not model:
        dst = os.path.join(os.path.abspath(a.out_dir or base), 'model_xmd', name + '.xmd')
        src = os.path.join(base, name + '.xmd')
        if not os.path.exists(dst) and os.path.exists(src):
            os.makedirs(os.path.dirname(dst), exist_ok=True)
            os.replace(src, dst)
            print('moved model to', dst)
        model = dst if os.path.exists(dst) else None
    c = Converter(a.xml, lib, joints, rig_root, os.path.abspath(a.out_dir or base), model, cp_config)
    if a.cp_template:
        # add this network's CPs to the settings file, then carry on with the normal run using it
        c.analyse()
        print('%s now lists %d control parameters' % (a.cp_config, c.write_cp_template(a.cp_config)))
        cp_config = {k: v for k, v in json.load(open(a.cp_config, encoding='utf-8')).items() if not k.startswith('_')}
        c = Converter(a.xml, lib, joints, rig_root, os.path.abspath(a.out_dir or base), model, cp_config)
    luas = c.run()
    mcn = os.path.join(c.out_dir, c.name + '.mcn')
    if a.cp_only:
        print('wrote %d CP groups into %s' % (c.inject_groups_only(mcn), mcn))
        print('wrote', c.cp_only()); return
    if a.inject_late:
        print('added %d stage-2 transitions to ActiveStates in %s' % (c.inject_late(mcn), mcn))
        for u in c.unsupported:
            if 'ActiveState' in u: print('UNSUPPORTED:', u)
        return
    if a.inject:
        n_as, n_pins, n_grp = c.inject(mcn)
        print('injected %d ActiveStates, %d state pass-down pins, %d CP groups into %s' % (n_as, n_pins, n_grp, mcn))
        return
    for lua in luas: print('wrote', lua)
    print('containers: %d (%d nested blend trees), pass-down pins: %d, stage-2 connections: %d' % (
        len(c.containers), sum(1 for k in c.containers.values() if k == 'nbt'), len(c.pin_defs), len(c.edges2)))
    for u in c.unsupported: print('UNSUPPORTED:', u)
    for u in sorted(c.unmapped): print('UNMAPPED FIELD:', u)
    if cp_config: print('CP config: %d entries, %d used by this network' % (len(cp_config), len(c.cp_used)))
    print('%d control parameters without a CP config entry -> build/%s_cp_config_missing.log' % (c.missing_cp, c.name))


if __name__ == '__main__':
    main()
