"""Merge ActiveStates in a morphemeConnect .mcn.

Usage: python mcn_merge_active_states.py <file.mcn> [--write] [--sm NAME ...] [--any-dest]

Without --write it only prints the plan. With --write the file is backed up to
<file>.bak_before_as_merge[_N] and rewritten in place (text-level edit, CRLF kept).

Grouping, per state machine, for ActiveStates whose transitions all share one key:
  1. Message first: ActiveStates whose transitions test the same messages (MessageCondition,
     not OnNotSet). Among them, CP test values that occur on more than one transition
     form their own group (Attack + AttackCategory 0-9 stays apart from 10-19); transitions whose
     CP test values occur only once are merged into one group by message (Die + DieCategory 20,
     60, 80... become one ActiveState_Die).
     Event conditions are ignored.
  2. No message: the key is the set of CP range tests (ControlParamInRange / ControlParamTest
     with their values and NotInRange flag). Event conditions are ignored.
  ActiveStates with neither a message nor a CP test are left alone.
The key also includes the ActiveState's own settings (AllStates / States) and, unless
--any-dest is given, the transition destination, so one ActiveState keeps one target.

A group keeps one ActiveState (the one with the most transitions, then the first by name).
All transitions of the other members are re-pointed to it and renamed <keeper>_<dst>[_n];
the other members are deleted. The keeper is renamed ActiveState_<messages or CP label>
(+ _<destination> when two groups in one state machine would get the same name).
The result is validated (same structure apart from removed nodes, every pointer resolves,
no duplicate sibling names) before anything is written.
"""
import sys, os, re, shutil, collections
import xml.etree.ElementTree as ET, xml.parsers.expat

GN = ('BlendTreeNode', 'StateMachineNode')
EVENTS = {'InDurationEvent', 'UserDataEvent', 'InEventRange', 'FractionThroughDurationEvent'}

def ntype(e):
    nt = e.find('NodeType'); return nt.text if nt is not None else None
def graph(e):
    ge = e.find('GraphEntry')
    return None if ge is None else ge.find('StateMachine')
def attr(c, name):
    for a in c.iter():
        if a.get('name') == name: return a
def aval(c, name, d=None):
    a = attr(c, name)
    if a is None: return d
    v = a.find('Value'); return v.text.strip() if v is not None and v.text else d
def num(x):
    f = float(x); return str(int(f)) if f == int(f) else str(f)
def last(ptr): return ptr.strip().split('.')[-1]

def messages(t):
    out = []
    for c in t.iter('Condition'):
        if c.find('Type').text == 'MessageCondition' and aval(c, 'OnNotSet') != '1':
            q = c.find('.//RequestEntry')
            if q is not None and q.text: out.append(last(q.text))
    return tuple(sorted(set(out)))
def cp_tests(t):
    out = []
    for c in t.iter('Condition'):
        ty = c.find('Type').text
        if ty not in ('ControlParamInRange', 'ControlParamTest'): continue
        p = c.find('.//ControlParameterDataPin')
        if p is None or not p.text: continue
        cp = p.text.strip().split('.')[-3]
        if ty == 'ControlParamInRange':
            out.append((cp, num(aval(c, 'LowerTestValue', '0')), num(aval(c, 'UpperTestValue', '0')), aval(c, 'NotInRange') == '1'))
        else:
            out.append((cp, ty) + tuple(sorted((a.get('name') or a.tag, (a.text or '').strip()) for a in c.iter() if (a.text or '').strip() and a.tag not in ('Type', 'ManifestVersion', 'ControlParameterDataPin'))))
    return tuple(sorted(out))
def cp_label(tests):
    o = []
    for t in tests:
        n = t[0].replace('_Ene', '').replace('Category', '')
        if len(t) == 4 and t[1] in ('0',) and t[2] != '0' and False: pass
        if len(t) == 4:
            o.append(('Not' if t[3] else '') + n + (t[1] if t[1] == t[2] else t[1] + 'to' + t[2]))
        else: o.append(n)
    return '_'.join(o)
def clean(s): return re.sub(r'[^A-Za-z0-9_]', '', s.replace('-', 'm').replace('.', 'p'))

def tkey(t, any_dest):
    dest = () if any_dest else (t.find('To').text.strip(),)
    m = messages(t); c = cp_tests(t)
    if m: return ('msg', m, c) + dest
    if c: return ('cp', c) + dest
    return None

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

def plan(root, pm, only, any_dest):
    def opath(e):
        s = []
        while e is not None and e.tag != 'NaturalMotion':
            s.append(e.get('name') or e.tag); e = pm.get(e)
        return '.'.join(reversed(s))
    groups = []
    for sm in root.iter():
        if sm.tag not in GN or ntype(sm) == 'ActiveState': continue
        g = graph(sm)
        if g is None or g.find('TransitionEdges') is None: continue
        if only and sm.get('name') not in only: continue
        out = collections.defaultdict(list)
        for t in g.find('TransitionEdges'): out[last(t.find('From').text)].append(t)
        nodes = g.find('StateMachineNodes')
        byk = collections.defaultdict(list)
        for a in (nodes if nodes is not None else []):
            if ntype(a) != 'ActiveState': continue
            ts = out.get(a.get('name'), [])
            ks = set(tkey(t, any_dest) for t in ts)
            if len(ks) != 1 or None in ks: continue
            byk[(as_settings(a), ks.pop())].append((a, ts))
        # message groups: split by CP tests where values are shared; if no two share values, merge by message
        bym = collections.defaultdict(list)
        gs = []
        for k, m in byk.items():
            if k[1][0] == 'msg': bym[(k[0], k[1][1]) + k[1][3:]].append((k, m))
            elif len(m) > 1: gs.append((k, m))
        for mk, subs in bym.items():
            # a value set is 'shared' when it occurs on more than one transition (several ActiveStates,
            # or one ActiveState already holding several transitions with those values)
            shared = [(k, m) for k, m in subs if sum(len(ts) for _, ts in m) > 1]
            single = [(k, m) for k, m in subs if sum(len(ts) for _, ts in m) == 1]
            gs += [(k, m) for k, m in shared if len(m) > 1]
            if len(single) > 1:
                k0 = single[0][0]
                gs.append(((k0[0], ('msg', k0[1][1], ()) + k0[1][3:]), [x for _, m in single for x in m]))
        gs = [(k, sorted(m, key=lambda x: (-len(x[1]), x[0].get('name')))) for k, m in gs]
        # names
        labels = {}
        for k, m in gs:
            kk = k[1]
            labels[id(m)] = '_'.join(kk[1]) if kk[0] == 'msg' else cp_label(kk[1])
            if kk[0] == 'msg' and kk[2]:
                cl = cp_label(kk[2])
                labels[id(m)] = cl if cl.startswith(labels[id(m)]) else labels[id(m)] + '_' + cl
        cnt = collections.Counter(labels.values())
        for k, m in gs:
            lab = labels[id(m)]
            if cnt[lab] > 1 and not any_dest:
                d = re.sub(r'^(BT|SM)_', '', last(k[1][-1]))
                if d != lab: lab += '_' + d
            groups.append((sm, opath(sm), k, m, 'ActiveState_' + clean(lab), opath))
    return groups

def main(fn, write, only, any_dest):
    data = open(fn, 'rb').read()
    root = ET.fromstring(data); pm = {c: p for p in root.iter() for c in p}
    groups = plan(root, pm, only, any_dest)
    if not groups: print('nothing to merge'); return
    sp = spans(data); lines = data.decode('utf-8').split('\r\n')
    delete = []; renames = []  # (old keeper path, new keeper path)
    by_sm = collections.defaultdict(list)
    for gr in groups: by_sm[gr[1]].append(gr)
    for smp, grs in by_sm.items():
        sm = grs[0][0]
        taken = set(a.get('name') for a in graph(sm).find('StateMachineNodes'))
        used = set(t.get('name') for t in graph(sm).find('TransitionEdges'))
        moving = [t for gr in grs for _, ts in gr[3] for t in ts]
        used -= set(t.get('name') for t in moving)
        for sm_, _, k, mem, newname, opath in grs:
            keep = mem[0][0]; kp = opath(keep)
            others = [a.get('name') for a, _ in mem[1:]]
            taken -= set(others) | {keep.get('name')}
            nn = newname; i = 1
            while nn in taken: nn = '%s_%d' % (newname, i); i += 1
            taken.add(nn)
            print('%s: %s <- %d ActiveStates (%s)' % (sm.get('name'), nn, len(mem), ', '.join(a.get('name') for a, _ in mem)))
            for a, ts in mem:
                ap = opath(a)
                for t in ts:
                    if a is not keep:
                        fl = sp[opath(t) + '.From'][0] - 1
                        assert ('>' + ap + '<') in lines[fl], opath(t)
                        lines[fl] = lines[fl].replace('>' + ap + '<', '>' + kp + '<')
                    base = nn + '_' + last(t.find('To').text); tn = base; j = 1
                    while tn in used: tn = '%s_%d' % (base, j); j += 1
                    used.add(tn)
                    tl = sp[opath(t)][0] - 1
                    assert lines[tl].count('name="%s"' % t.get('name')) == 1
                    lines[tl] = lines[tl].replace('name="%s"' % t.get('name'), 'name="%s"' % tn)
                if a is not keep: delete.append(sp[ap])
            if nn != keep.get('name'):
                kl = sp[kp][0] - 1
                lines[kl] = lines[kl].replace('name="%s"' % keep.get('name'), 'name="%s"' % nn)
                renames.append((kp, kp.rsplit('.', 1)[0] + '.' + nn))
    for s, e in sorted(delete, reverse=True): del lines[s - 1:e]
    txt = '\r\n'.join(lines)
    for old, new in renames:
        txt = txt.replace('>' + old + '<', '>' + new + '<').replace('>' + old + '.', '>' + new + '.')
    bad, dup = validate(txt)
    print('%d groups, %d ActiveStates removed; unresolved pointers %d, duplicate names %d' % (len(groups), len(delete), len(bad), len(dup)))
    for b in bad[:5]: print('  unresolved', b)
    for d in dup[:5]: print('  duplicate', d)
    if bad or dup: print('NOT written'); return
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
    main(a[0], '--write' in a, only, '--any-dest' in a)
