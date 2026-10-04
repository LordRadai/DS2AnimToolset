"""
xmldiff.py - semantic diff of two morpheme network export XMLs (original vs Connect re-export).

Node ids differ between the two files, so every NetworkNodeId value is translated to a node name
before comparing, and element order is ignored.  Nodes are matched by export name; with --paths
(<name>_paths.lua written by the rebuild scripts) they are matched through the Connect path each
original node really got, which covers duplicate names Connect had to suffix and nodes the
converter had to move.  Name changes are listed separately: a changed *named* node (one whose
name is not decompiler-generated) is an error, since the game looks nodes up by name.

Usage: python xmldiff.py original.xml roundtrip.xml [--paths X_paths.lua] [--max N]
"""
import argparse, collections, os, re, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mcnxml

ID_TYPES = ('NetworkNodeId',)
IGNORE = {'ClipRangeMode_1'}
EQUIV = {('DeltaTrajSource', 0): 3}
TRANSITS = (400, 402, 403, 440, 441)
CPS = (20, 21, 22, 23, 24, 25)
# field names the DS2 decompiler writes differently from Connect 3.6.2
ALIASES = {'InitValue_X': 'InitValueX', 'InitValue_Y': 'InitValueY', 'InitValue_Z': 'InitValueZ'}


def generated(leaf, nid=None):
    """Names the decompiler invents (no name in the NMB string table)."""
    return bool(re.fullmatch(r'[A-Za-z0-9]+_\d+(_\d+)?|ActiveState\w*', leaf)) or bool(re.fullmatch(r'a\d\d_\d\d_\d{4}_.*', leaf))


class Net:
    def __init__(self, path):
        self.root, self.nodes, msgs = mcnxml.load(path)
        self.msgs = {int(m['messageID']): m['name'] for m in msgs}
        self.sm_names = {n.name for n in self.nodes.values() if n.type == 10}
        self.anims = {}
        lib = os.path.join(os.path.dirname(os.path.abspath(path)), os.path.splitext(os.path.basename(path))[0] + '_Library.xml')
        if os.path.exists(lib):
            import xml.etree.ElementTree as ET
            s = ET.parse(lib).getroot().find('AnimationSet')
            for e in s.findall('AnimationEntry'):
                self.anims[int(e.get('index'))] = '%s|%s|%s|%s|opts=%r' % (
                    e.findtext('animFile'), e.findtext('take'), e.findtext('syncTrack'), e.get('format'), e.get('options'))
        self.rename = {}   # id -> name used for comparison (set for the original from --paths)

    def nm(self, nid):
        n = self.nodes.get(nid)
        if n is None: return '<invalid>' if nid in (0xFFFFFFFF, 0xFFFF, -1, None) else '<none %r>' % nid
        return self.rename.get(nid, n.name)

    def state(self, nid):
        name = self.nm(nid)
        comps = name.split('|')
        sms = {self.nm(i) for i, n in self.nodes.items() if n.type == 10}
        for k in range(len(comps) - 1, 0, -1):
            if '|'.join(comps[:k]) in sms: return '|'.join(comps[:k + 1])
        return name

    def cond_sig(self, c):
        items = []
        for d, t, v, a in c.elems:
            if d == 'MessageID': v = self.msgs.get(v, v)
            elif t in ID_TYPES or d == 'NodeID': v = self.nm(v)
            elif isinstance(v, float): v = round(v, 5)
            items.append((d, v))
        return (c.type, tuple(sorted(items, key=lambda x: x[0])))

    def transit_conditions(self):
        out = collections.defaultdict(list)
        for h in self.nodes.values():
            for tid, idx in h.condsets:
                out[tid].append(tuple(self.cond_sig(h.conditions[i]) for i in idx))
            for tid, idx in h.common_condsets:
                out[tid].append(('<common>',) + tuple(self.cond_sig(h.common_conditions[i]) for i in idx))
        return out

    def norm_elems(self, n):
        out = {}
        for d, t, v, a in n.elems:
            if d in IGNORE: continue
            d = ALIASES.get(d, d)
            if d == 'AnimIndex': v = self.anims.get(v, 'index %r' % v)
            elif t in ID_TYPES or re.match(r'DestinationSubState(Parent)?ID_', d): v = self.nm(v)
            elif d.startswith('EmittedMessageID') or d == 'MessageID': v = self.msgs.get(v, v)
            elif isinstance(v, float): v = round(v, 4)
            elif isinstance(v, bytes): v = v.hex()
            v = EQUIV.get((d, v), v)
            if d.startswith('RuntimeChildNodeID_') or d.startswith('RuntimeChildTransitID_'):
                out.setdefault(d.split('_')[0] + '*', []).append(v); continue
            while d in out: d += '#'   # same description twice (e.g. HipsIK FootTurnWeight pin + attribute)
            out[d] = v
        for k in list(out):
            if k.endswith('*'): out[k] = sorted(out[k])
        return out


def load_paths(path):
    P = {}
    for line in open(path, encoding='latin1'):
        m = re.match(r'P\[(?:(\d+)|"(.*)")\] = "(.*)"$', line.strip())
        if m and m.group(1): P[int(m.group(1))] = m.group(3).replace('\\\\', '\\')
    return P


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('original'); ap.add_argument('roundtrip')
    ap.add_argument('--paths'); ap.add_argument('--max', type=int, default=200)
    a = ap.parse_args()
    A, B = Net(a.original), Net(a.roundtrip)
    issues, name_changes, named_changes = [], [], []
    if a.paths:
        P = load_paths(a.paths)
        for nid, n in A.nodes.items():
            if nid in P and P[nid] != n.name:
                A.rename[nid] = P[nid]
                gen = not n.name or n.type in TRANSITS or all(generated(c) for c in set(n.name.split('|')) - set(P[nid].split('|')))
                (name_changes if gen else named_changes).append((n.name, P[nid]))
    for k in sorted(set(A.msgs) | set(B.msgs)):
        if A.msgs.get(k) != B.msgs.get(k): issues.append('message %d: %r vs %r' % (k, A.msgs.get(k), B.msgs.get(k)))

    bname = collections.defaultdict(list)
    for n in B.nodes.values(): bname[n.name].append(n)
    pairs, used = [], set()
    for n in sorted(A.nodes.values(), key=lambda n: n.id):
        want = A.nm(n.id)
        cand = [m for m in bname.get(want, []) if id(m) not in used and m.type == n.type]
        if n.type == 9: cand = [m for m in B.nodes.values() if m.type == 9]
        if not cand and n.type in TRANSITS:   # unnamed match: same state machine + endpoints
            key = (A.nm(n.parent), A.state(n.get('SourceNodeID')) if n.get('SourceNodeID') is not None else None, A.state(n.get('DestNodeID')))
            cand = [m for m in B.nodes.values() if m.type == n.type and id(m) not in used and
                    (B.nm(m.parent), B.state(m.get('SourceNodeID')) if m.get('SourceNodeID') is not None else None, B.state(m.get('DestNodeID'))) == key]
        if not cand:
            issues.append('MISSING node #%d %s (type %d)' % (n.id, want, n.type)); continue
        used.add(id(cand[0])); pairs.append((n, cand[0]))
    for n in B.nodes.values():
        if id(n) not in used: issues.append('EXTRA node in roundtrip: %s (type %d)' % (n.name, n.type))

    ca, cb = A.transit_conditions(), B.transit_conditions()
    nfield, per_kind, presence = 0, collections.Counter(), collections.Counter()
    for x, y in pairs:
        if x.type == 9: continue
        if x.type not in CPS and A.nm(x.parent) != B.nm(y.parent) and not (x.parent in A.nodes and A.nodes[x.parent].type == 9):
            issues.append('%s: downstream parent %s vs %s' % (A.nm(x.id), A.nm(x.parent), B.nm(y.parent))); per_kind['parent'] += 1
        if (x.attrs.get('downstreamMultiplyConnected') == 'true') != (y.attrs.get('downstreamMultiplyConnected') == 'true'):
            issues.append('%s: downstreamMultiplyConnected %s vs %s' % (A.nm(x.id), x.attrs.get('downstreamMultiplyConnected'), y.attrs.get('downstreamMultiplyConnected')))
            per_kind['multiplyConnected'] += 1
        ea, eb = A.norm_elems(x), B.norm_elems(y)
        for k in sorted(set(ea) | set(eb)):
            nfield += 1
            if (k in ea) != (k in eb):   # field only one side writes: the DS2 build's manifests differ from vanilla 3.6.2
                presence['type %d %s only in %s' % (x.type, re.sub(r'\d+', 'N', k), 'original' if k in ea else 'Connect')] += 1
                continue
            if ea.get(k) != eb.get(k):
                issues.append('%s (type %d): %s = %r vs %r' % (A.nm(x.id), x.type, k, ea.get(k), eb.get(k))); per_kind['type %d %s' % (x.type, re.sub(r'\d+', 'N', k))] += 1
        sa, sb = sorted(ca.get(x.id, [])), sorted(cb.get(y.id, []))
        if sa != sb:
            issues.append('%s: conditions\n    orig %r\n    new  %r' % (A.nm(x.id), sa, sb)); per_kind['conditions'] += 1

    print('matched %d/%d nodes, compared %d fields, %d differences' % (len(pairs), len(A.nodes), nfield, len(issues)))
    if named_changes:
        print('NAMED NODES WITH CHANGED NAMES (%d) - the game may not find these:' % len(named_changes))
        for o, n in named_changes[:a.max]: print('   %s  ->  %s' % (o, n))
    if name_changes:
        print('%d generated (decompiler) names changed: duplicates suffixed by Connect or moved nodes' % len(name_changes))
    if per_kind:
        print('differences by kind:')
        for k, v in per_kind.most_common(): print('   %5d  %s' % (v, k))
    if presence:
        print('fields written by only one side (manifest differences, not value errors):')
        for k, v in presence.most_common(): print('   %5d  %s' % (v, k))
    for i in issues[:a.max]: print(' -', i)
    return 0 if not issues and not named_changes else 1


if __name__ == '__main__':
    sys.exit(main())
