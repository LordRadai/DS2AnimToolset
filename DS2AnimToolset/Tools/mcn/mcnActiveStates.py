"""Split / merge ActiveStates in a morphemeConnect .mcn: one ActiveState per destination.

Usage: python mcnMergeActiveStates.py <file.mcn> [--write] [--sm NAME ...]

Without --write it only prints the plan. With --write the file is backed up to
<file>.bak_before_as_merge[_N] and rewritten in place (text-level edit, CRLF kept).

Per state machine, the transitions leaving ActiveStates are grouped by destination:
  * an ActiveState whose transitions go to several destinations is split, one ActiveState each;
  * ActiveStates whose transitions go to the same destination are merged into one.
The key also includes the ActiveState's own settings (AllStates / States): those decide which states the transitions
fire from, so ActiveStates with the same destination but different source lists stay separate (merging them would let
transitions fire from states that never had them). They are reported.

Every ActiveState is named ActiveState (ActiveState_1, ActiveState_2, ... where a state machine has several, since
sibling names must be unique); authored names are dropped. ActiveStates without transitions are only renamed.
Transitions are renamed <ActiveState>_<destination>[_n]; every pointer to a renamed transition (e.g. ActiveState
'States' lists) is rewired. The result is validated (every pointer resolves, no duplicate sibling names) before
anything is written.
"""
import sys, os, shutil, collections
import xml.etree.ElementTree as ET, xml.parsers.expat

GN = ('BlendTreeNode', 'StateMachineNode')
BASE = 'ActiveState'

def ntype(e):
    nt = e.find('NodeType'); return nt.text if nt is not None else None
def graph(e):
    ge = e.find('GraphEntry')
    return None if ge is None else ge.find('StateMachine')
def last(ptr): return ptr.strip().split('.')[-1]

def as_settings(a):
    at = a.find('Attributes')
    return tuple((x.get('name') or x.tag, (x.text or '').strip()) for x in at.iter()) if at is not None else ()

def spans(data):
    """element path (dotted, from MorphemeDB) -> (start line, end line)"""
    p = xml.parsers.expat.ParserCreate(); st = []; out = {}
    def start(tag, a):
        parent = st[-1][0] if st else None
        seg = a.get('name') or tag
        path = parent + '.' + seg if parent else ('MorphemeDB' if tag == 'MorphemeDB' else None)
        st.append((path, p.CurrentLineNumber))
    def end(tag):
        path, ln = st.pop()
        if path: out[path] = (ln, p.CurrentLineNumber)
    p.StartElementHandler = start; p.EndElementHandler = end; p.Parse(data, True)
    return out

def validate(txt):
    root = ET.fromstring(txt.encode('utf-8')); paths = set(); dup = []
    def walk(e, pre):
        seen = set()
        for c in e:
            s = c.get('name') or c.tag; p = pre + '.' + s; paths.add(p)
            if c.get('name'):
                if s in seen: dup.append(p)
                seen.add(s)
            walk(c, p)
    m = root if root.tag == 'MorphemeDB' else root.find('MorphemeDB')
    walk(m, 'MorphemeDB'); paths.add('MorphemeDB')
    bad = [e.text.strip() for e in root.iter() if (e.get('type') == 'pointer' or e.tag == 'elem') and (e.text or '').strip().startswith('MorphemeDB') and e.text.strip() not in paths]
    return bad, dup

def plan(root, pm, only):
    """[(state machine, [ActiveState], [(source ActiveState, destination pointer, [transitions])], opath)]"""
    def opath(e):
        s = []
        while e is not None and e.tag != 'NaturalMotion':
            s.append(e.get('name') or e.tag); e = pm.get(e)
        return '.'.join(reversed(s))
    out = []
    for sm in root.iter():
        if sm.tag not in GN or ntype(sm) == 'ActiveState': continue
        g = graph(sm)
        if g is None or g.find('StateMachineNodes') is None: continue
        if only and sm.get('name') not in only: continue
        actives = [a for a in g.find('StateMachineNodes') if ntype(a) == 'ActiveState']
        if not actives: continue
        leaving = collections.defaultdict(list)
        edges = g.find('TransitionEdges')
        for t in (edges if edges is not None else []):
            leaving[last(t.find('From').text)].append(t)
        groups = collections.OrderedDict()
        for a in actives:
            for t in leaving.get(a.get('name'), []):
                key = (as_settings(a), t.find('To').text.strip())
                if key not in groups: groups[key] = (a, [])
                groups[key][1].append(t)
        out.append((sm, actives, [(a, k[1], ts) for k, (a, ts) in groups.items()], leaving, opath))
    return out

def main(fn, write, only):
    data = open(fn, 'rb').read()
    root = ET.fromstring(data); pm = {c: p for p in root.iter() for c in p}
    sms = plan(root, pm, only)
    if not sms: print('no ActiveStates'); return
    sp = spans(data); lines = data.decode('utf-8').split('\r\n')
    edits = {}            # line index -> new text
    inserts = []          # (insert at line index, [lines]) clones for split ActiveStates
    delete = []           # (start, end) spans of merged-away ActiveStates
    renames = []          # transition (old path, new path)
    changed_any = False
    for sm, actives, groups, leaving, opath in sms:
        g = graph(sm)
        nodes_path = opath(g.find('StateMachineNodes'))
        idle = [a for a in actives if not leaving.get(a.get('name'))]
        # names: ActiveState, ActiveState_1, ... avoiding the state machine's other nodes
        taken = set(n.get('name') for n in g.find('StateMachineNodes') if ntype(n) != 'ActiveState')
        names, k = [], 0
        while len(names) < len(groups) + len(idle):
            nm = BASE if k == 0 else '%s_%d' % (BASE, k)
            if nm not in taken: names.append(nm)
            k += 1
        # skip state machines already in shape
        cur = sorted((a.get('name'), sorted(t.get('name') for t in leaving.get(a.get('name'), []))) for a in actives)
        planned = sorted([(names[i], sorted(t.get('name') for t in groups[i][2])) for i in range(len(groups))] +
                         [(names[len(groups) + j], []) for j in range(len(idle))])
        tnames_ok = all(t.get('name').startswith(names[i] + '_') for i in range(len(groups)) for t in groups[i][2])
        if planned == cur and tnames_ok: continue
        changed_any = True
        # each group gets an ActiveState block: its source ActiveState the first time, a copy of it after that
        reused = set()
        dests = collections.Counter(dest for _, dest, _ in groups)
        apart = [d for d, n in dests.items() if n > 1]
        print('%s: %d ActiveStates -> %d%s' % (
            sm.get('name'), len(actives), len(groups) + len(idle),
            ('; kept apart (different source lists) for: %s' % ', '.join(last(d) for d in apart)) if apart else ''))
        used = set(t.get('name') for t in g.find('TransitionEdges'))
        used -= set(t.get('name') for _, _, ts in groups for t in ts)
        for i, (src, dest, ts) in enumerate(groups):
            nn = names[i]
            new_ptr = nodes_path + '.' + nn
            s, e = sp[opath(src)]
            if src.get('name') in reused:
                blk = lines[s - 1:e]
                blk[0] = blk[0].replace('name="%s"' % src.get('name'), 'name="%s"' % nn, 1)
                inserts.append((e, blk))
            else:
                reused.add(src.get('name'))
                edits[s - 1] = lines[s - 1].replace('name="%s"' % src.get('name'), 'name="%s"' % nn, 1)
            print('   %s -> %s  (from %s)' % (nn, last(dest), ', '.join(sorted(set(last(t.find('From').text) for t in ts)))))
            for t in ts:
                fl = sp[opath(t) + '.From'][0] - 1
                old_from = t.find('From').text.strip()
                assert ('>' + old_from + '<') in lines[fl], opath(t)
                edits[fl] = lines[fl].replace('>' + old_from + '<', '>' + new_ptr + '<')
                base = nn + '_' + last(dest); tn = base; j = 1
                while tn in used: tn = '%s_%d' % (base, j); j += 1
                used.add(tn)
                if tn != t.get('name'):
                    tp = opath(t); renames.append((tp, tp.rsplit('.', 1)[0] + '.' + tn))
                    tl = sp[tp][0] - 1
                    assert lines[tl].count('name="%s"' % t.get('name')) == 1
                    edits[tl] = edits.get(tl, lines[tl]).replace('name="%s"' % t.get('name'), 'name="%s"' % tn)
        for j, a in enumerate(idle):
            s, e = sp[opath(a)]
            edits[s - 1] = lines[s - 1].replace('name="%s"' % a.get('name'), 'name="%s"' % names[len(groups) + j], 1)
        for a in actives:
            if a not in idle and a.get('name') not in reused: delete.append(sp[opath(a)])
    if not changed_any: print('nothing to change'); return
    for i, v in edits.items(): lines[i] = v
    # inserts and deletes from the bottom up, so earlier line numbers stay valid
    ops = [(e, 0, blk) for e, blk in inserts] + [(s, 1, e) for s, e in delete]
    for pos, kind, x in sorted(ops, key=lambda o: o[0], reverse=True):
        if kind == 0: lines[pos:pos] = x
        else: del lines[pos - 1:x]
    txt = '\r\n'.join(lines)
    # rewire every pointer to a renamed transition (two-phase through placeholders so swapped names can't collide)
    for i, (old, new) in enumerate(renames):
        txt = txt.replace('>' + old + '<', '>\x00%d\x00<' % i).replace('>' + old + '.', '>\x00%d\x00.' % i)
    for i, (old, new) in enumerate(renames):
        txt = txt.replace('>\x00%d\x00' % i, '>' + new)
    bad, dup = validate(txt)
    print('%d ActiveStates merged away, %d added by splits; unresolved pointers %d, duplicate names %d' % (
        len(delete), len(inserts), len(bad), len(dup)))
    for b in bad[:5]: print('  unresolved', b)
    for d in dup[:5]: print('  duplicate', d)
    if bad or dup: print('NOT written'); return 1
    if write:
        bak = fn + '.bak_before_as_merge'; i = 1
        while os.path.exists(bak): bak = '%s.bak_before_as_merge_%d' % (fn, i); i += 1
        shutil.copy2(fn, bak)
        with open(fn, 'w', encoding='utf-8', newline='') as f: f.write(txt)
        print('written; backup', bak)
    else: print('dry run, nothing written (use --write)')

if __name__ == '__main__':
    a = sys.argv[1:]
    if not a or a[0].startswith('-'): print(__doc__); sys.exit(1)
    only = set(a[i + 1] for i, x in enumerate(a) if x == '--sm')
    sys.exit(main(a[0], '--write' in a, only) or 0)
