"""
hierarchy.py - recover the Connect hierarchy (export path of every node) from the runtime data alone.

The NMB only names a few nodes.  Everything else is placed from what the compiled network itself says:
  * state machines list their states' runtime roots (RuntimeChildNodeID_*) -> SM / state structure
  * a node that feeds exactly one consumer lives in its consumer's graph (downstreamParentID)
  * a multiply connected node lives next to its requester (downstreamParentID); when the requester is a
    state machine it lives in that SM's state holding its consumers, which reach it through one-to-many
    pass-down pins (Connect makes the requester whatever the pass-down container's result feeds)
  * data nodes (operators) go to the lowest graph above all their consumers
Container names come from the named nodes' paths where they pass through them; the rest get the usual
generated names (StateMachine_<id>, BlendTree_<root>_0, <Type>_<id>); AnimWithEvents take the anim name.
"""
import collections, os, re

CP_TYPES = (20, 21, 22, 23, 24, 25)
DATA_TYPES = (110, 112, 142, 144, 146)
SM, NETWORK = 10, 9
TRANSITS = (400, 402, 403)
ROOT = ('root',)


class Hierarchy:
    def __init__(self, nodes, root_id, spec, anim_name):
        self.nodes, self.root_id, self.spec, self.anim_name = nodes, root_id, spec, anim_name
        self.log = []
        self.sms = {i: n for i, n in nodes.items() if n.type == SM}
        self.state_owner = {}          # state root id -> owning SM id
        self.states = collections.defaultdict(list)
        for s in self.sms.values():
            for d, t, v, a in s.elems:
                if d.startswith('RuntimeChildNodeID_') and v in nodes:
                    self.state_owner[v] = s.id; self.states[s.id].append(v)
        self.consumers = collections.defaultdict(list)
        for n in nodes.values():
            if n.id in spec:
                for field, pin in spec[n.id][2]:
                    if n.get(field) in nodes: self.consumers[n.get(field)].append(n.id)
        self.G = {}
        self.override, self.nbt_depth = {}, {}

    # graphs: ROOT or ('state', sm, root).  A state whose root is a SM is only a real BlendTree ("wrapper")
    # when something else lives in it or a named path shows it; otherwise the SM itself is the state.
    def graph(self, nid, stack=()):
        if nid in self.G: return self.G[nid]
        if nid in self.override: self.G[nid] = self.override[nid]; return self.G[nid]
        if nid in stack: raise RuntimeError('placement cycle at %d' % nid)
        stack = stack + (nid,)
        n = self.nodes[nid]
        if nid in self.state_owner:
            g = ('state', self.state_owner[nid], nid)
        elif nid == self.root_id:
            g = ROOT
        elif n.type in DATA_TYPES or n.parent not in self.nodes:
            gs = [self.graph(c, stack) for c in self.consumers.get(nid, [])]
            g = self.lca(gs) if gs else ROOT
        else:
            p = self.nodes[n.parent]
            if p.type == NETWORK:
                g = ROOT
            elif p.type == SM and nid not in self.state_owner:
                # requester is a state machine: the state of p that holds the consumers
                gs = [self.graph(c, stack) for c in self.consumers.get(nid, [])]
                g = None
                for st in self.states[p.id]:
                    sg = ('state', p.id, st)
                    if gs and all(self.within(x, sg) for x in gs): g = sg; break
                if g is None:
                    self.log.append('node %d: requester SM %d has no state holding all consumers' % (nid, p.id))
                    g = self.lca(gs) if gs else ('state', p.id, self.states[p.id][0])
            else:
                g = self.graph(p.id, stack)
        self.G[nid] = g
        return g

    def nested_containers(self, named=()):
        """Multiply connected nodes whose requester P is an ordinary node (or the network).  Connect makes the
        requester whatever the pass-down container's result feeds, so their consumers sit in nested blend
        trees rooted at P's input R; a chain N1 -> N2 -> ... of such nodes with the same requester nests
        one level per link (each container is the root of the one around it, so they all resolve to P)."""
        nodes = self.nodes
        groups = collections.defaultdict(list)
        for n in nodes.values():
            if n.attrs.get('downstreamMultiplyConnected') != 'true' or n.type in CP_TYPES or n.type in DATA_TYPES: continue
            p = nodes.get(n.parent)
            if p is None or p.type == SM or n.id in self.state_owner or n.id in named: continue   # named nodes stay put
            groups[n.parent].append(n.id)
        for P, members in groups.items():
            mset = set(members)
            def walk(c):
                """Last node before P on c's parent chain (stops early at a group member)."""
                seen, x = set(), c
                while x in nodes and x not in seen:
                    seen.add(x)
                    px = nodes[x].parent
                    if px == P or x in mset: return x
                    x = px
                return None
            inside = collections.defaultdict(set)   # member -> members it consumes (directly or via subtree)
            roots = set()
            for m in members:
                for c in self.consumers.get(m, []):
                    w = c if c in mset else walk(c)
                    if w is None: continue
                    if w in mset and w != m: inside[w].add(m)
                    elif w not in mset: roots.add(w)
            if not roots: continue
            if len(roots) > 1:
                self.log.append('requester %d: members reach several inputs %r' % (P, sorted(roots)))
            R = min(roots)
            if R in named:
                self.log.append('requester %d: input %d is a named node, nesting skipped' % (P, R)); continue
            level = {}
            def lev(m, stack=()):
                if m in level: return level[m]
                if m in stack: return 1
                level[m] = 1 + max([lev(x, stack + (m,)) for x in inside[m]] or [0])
                return level[m]
            for m in members: lev(m)
            L = max(level.values())
            self.nbt_depth[(P, R)] = L
            for m in members:
                if level[m] > 1: self.override[m] = ('nbt', P, R, level[m] - 1)
                else: self.override[m] = ROOT if nodes[P].type == NETWORK else None
            for m in [m for m in members if self.override[m] is None]: del self.override[m]   # beside P: normal rule
            self.override[R] = ('nbt', P, R, L)

    def parent_graph(self, g):
        """Graph that contains graph g's owner (the SM of a state)."""
        if g == ROOT: return None
        if g[0] == 'nbt':
            _, P, R, l = g
            if l > 1: return ('nbt', P, R, l - 1)
            return ROOT if self.nodes[P].type == NETWORK else self.graph(P)
        s = g[1]
        if s in self.state_owner:                      # the SM is itself a state root
            return ('state', self.state_owner[s], s)  # (wrapper or not, its state graph is the parent level)
        return self.graph(s)

    def ancestors(self, g):
        out = []
        while g is not None: out.append(g); g = self.parent_graph(g)
        return out

    def within(self, g, outer):
        return outer in self.ancestors(g)

    def lca(self, gs):
        common = None
        for g in gs:
            a = self.ancestors(g)
            common = a if common is None else [x for x in common if x in a]
        return common[0] if common else ROOT

    # ---------------------------------------------------------------- naming
    def build(self, named, state_names=None):
        """named: {node id: export name}; state_names: {(sm id, state root id): path of the container holding
        the root} (NodeIDNamesTable StateNode entries).  Returns {node id: path}."""
        nodes = self.nodes
        self.nested_containers(set(named))
        for i in nodes:
            if nodes[i].type not in CP_TYPES and nodes[i].type not in TRANSITS and nodes[i].type != NETWORK:
                self.graph(i)
        # wrappers: SM state roots with other nodes in their state graph
        occupied = collections.Counter(g for i, g in self.G.items() if not (g[0] == 'state' and g[2] == i))
        self.wrapped = {r for r in self.state_owner if r in self.sms and occupied[('state', self.state_owner[r], r)]}
        self.sm_name, self.state_name = {}, {}
        # names along named paths
        for nid, name in named.items():
            if nid not in nodes or nodes[nid].type in CP_TYPES: continue
            chain = self.chain(nid)               # entities from the root down to the node itself
            comps = name.split('|')
            if len(comps) != len(chain):
                # a named state whose BlendTree wraps the SM: SubAct|BT_MoveJump|SM_MoveJump
                if len(comps) == len(chain) + 1 and nid in self.state_owner and nid in self.sms:
                    self.wrapped.add(nid); chain = self.chain(nid)
                if len(comps) != len(chain):
                    self.log.append('named node %d %s: %d path components but %d structural levels %r' % (nid, name, len(comps), len(chain), chain))
                    continue
            self.assign(chain, comps)
        # state entries name the container holding the state's root: the BlendTree state itself, or for a
        # state machine state the SM that owns it (or, one level deeper, the BlendTree wrapping it)
        for (sm, r), name in sorted((state_names or {}).items()):
            if sm not in self.sms or self.state_owner.get(r) != sm:
                self.log.append('state entry %d in %d: not a state of that state machine' % (r, sm)); continue
            comps = name.split('|')
            if r in self.sms:
                base = self.chain(sm)
                if len(comps) == len(base) + 1: self.wrapped.add(r)
                chain = base if len(comps) == len(base) else self.chain_of_graph(('state', sm, r))
            else:
                chain = self.chain_of_graph(('state', sm, r))
            if len(comps) != len(chain):
                self.log.append('state entry %d %s: %d path components but %d structural levels' % (r, name, len(comps), len(chain)))
                continue
            self.assign(chain, comps)
        paths = {i: self.path(i) for i in nodes if nodes[i].type not in CP_TYPES + TRANSITS and nodes[i].type != NETWORK}
        for t in nodes.values():                 # transitions: '<source state>_<destination state>' in their SM
            if t.type not in TRANSITS: continue
            sm = t.parent
            src, dst = t.get('SourceNodeID'), t.get('DestNodeID')
            sname = self.state_path(sm, src).split('|')[-1] if src in self.state_owner else 'ActiveState'
            dname = self.state_path(sm, dst).split('|')[-1] if dst in self.state_owner else 'Unknown'
            paths[t.id] = '%s|%s_%s' % (self.path(sm), sname, dname)
        return paths

    def assign(self, chain, comps):
        for ent, comp in zip(chain, comps):
            if ent[0] not in ('sm', 'state'): continue
            tgt = self.sm_name if ent[0] == 'sm' else self.state_name
            key = ent[1] if ent[0] == 'sm' else (ent[1], ent[2])
            if tgt.get(key, comp) != comp:
                self.log.append('conflicting names for %r: %s vs %s' % (ent, tgt[key], comp))
            tgt[key] = comp

    def state_path(self, sm, rid):
        if rid in self.sms and rid not in self.wrapped: return self.path(rid)
        return '|'.join(self.part(e) for e in self.chain_of_graph(('state', sm, rid)))

    def part(self, ent):
        if ent[0] == 'nbt': return 'BlendTree_%d_%d' % (ent[2], self.nbt_depth[(ent[1], ent[2])] - ent[3])
        if ent[0] == 'sm': return self.sm_leaf(ent[1])
        if ent[0] == 'state': return self.state_leaf(ent[1], ent[2])
        return self.leaf(ent[1])

    def chain(self, nid):
        """Structural entities from the root to node nid: ('sm', id), ('state', sm, root), ('node', id)."""
        n = self.nodes[nid]
        if n.type == SM:
            if nid in self.state_owner and nid not in self.wrapped:
                return self.chain(self.state_owner[nid]) + [('sm', nid)]
            return self.chain_of_graph(self.graph(nid)) + [('sm', nid)]
        return self.chain_of_graph(self.graph(nid)) + [('node', nid)]

    def chain_of_graph(self, g):
        if g == ROOT: return []
        if g[0] == 'nbt': return self.chain_of_graph(self.parent_graph(g)) + [g]
        return self.chain(g[1]) + [('state', g[1], g[2])]

    def sm_leaf(self, s): return self.sm_name.get(s, 'StateMachine_%d' % s)

    def state_leaf(self, s, r):
        if (s, r) in self.state_name: return self.state_name[(s, r)]
        if r in self.sms and r not in self.wrapped: return self.sm_leaf(r)
        return 'BlendTree_%d_0' % r

    def leaf(self, nid):
        n = self.nodes[nid]
        if n.type == SM: return self.sm_leaf(nid)
        if n.type == 104:
            nm = self.anim_name(n)
            if nm: return nm
        return '%s_%d' % (self.spec[nid][0] if nid in self.spec else 'Node', nid)

    def path(self, nid):
        return '|'.join(self.part(e) for e in self.chain(nid))


def transition_name(paths, nodes, t, state_of):
    src = t.get('SourceNodeID')
    dst = state_of(t.parent, t.get('DestNodeID'))
    s = state_of(t.parent, src) if src is not None else 'ActiveState'
    leaf = lambda p: p.split('|')[-1] if p else 'Unknown'
    return '%s_%s' % (leaf(s) if src is not None else 'ActiveState', leaf(dst))
