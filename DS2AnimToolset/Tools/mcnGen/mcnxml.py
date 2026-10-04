"""Parse a morpheme export NetworkDefinition XML into decoded Python structures."""
import struct, xml.etree.ElementTree as ET

def decode(t, hexs):
    b = bytes(int(x, 16) for x in hexs.strip().strip(',').split(',') if x.strip()) if hexs and hexs.strip() else b''
    if t in ('float',): return struct.unpack('<f', b)[0]
    if t in ('int',): return struct.unpack('<i', b)[0]
    if t == 'char': return b.decode('latin1')
    if t in ('uint', 'NetworkNodeId', 'MessageID', 'AnimationId', 'AnimationID', 'NodeID'):
        return struct.unpack('<I', b)[0] if len(b) == 4 else int.from_bytes(b, 'little')
    if t == 'bool': return bool(b[0]) if b else False
    if t == 'double': return struct.unpack('<d', b)[0]
    if t == 'string': return b.split(b'\0')[0].decode('latin1')
    return b

def parse_block(db):
    if db is None: return []
    return [(de.get('description'), de.get('type'), decode(de.get('type'), de.text), dict(de.attrib)) for de in db.findall('DataElement')]

def parse_indices(t):
    return [int(x) for x in (t or '').replace(' ', '').split(',') if x != '']

class Cond:
    def __init__(s, e):
        s.type = int(e.get('typeID')); s.elems = parse_block(e.find('DataBlock'))
    def get(s, desc, default=None):
        for d, t, v, a in s.elems:
            if d == desc: return v
        return default

class Node:
    def __init__(s, e):
        s.e = e
        s.name = e.get('name'); s.id = int(e.get('networkID')); s.type = int(e.get('typeID'))
        s.parent = int(e.get('downstreamParentID')); s.attrs = dict(e.attrib)
        s.elems = []  # list of (desc, type, value, rawattrs)
        s.elems = parse_block(e.find('DataBlock'))
        s.extra = [c for c in e if c.tag != 'DataBlock']
        # conditions are stored on the transition's source state; common ones on the state machine
        s.conditions = {int(c.get('index')): Cond(c) for c in e.findall('Condition')}
        s.condsets = [(int(c.get('targetNodeID')), parse_indices(c.findtext('conditionIndices'))) for c in e.findall('ConditionSet')]
        s.common_conditions = {int(c.get('index')): Cond(c) for c in e.findall('CommonCondition')}
        s.common_condsets = [(int(c.get('targetNodeID')), parse_indices(c.findtext('conditionIndices'))) for c in e.findall('CommonConditionSet')]
    def get(s, desc, default=None, nth=0):
        k = 0
        for d, t, v, a in s.elems:
            if d == desc:
                if k == nth: return v
                k += 1
        return default
    def all(s, desc): return [v for d, t, v, a in s.elems if d == desc]
    def short(s): return s.name.split('|')[-1]

def load(path):
    r = ET.parse(path).getroot()
    nodes = {n.id: n for n in (Node(e) for e in r.findall('Node'))}
    msgs = [dict(m.attrib) for m in r.findall('Message')]
    return r, nodes, msgs

if __name__ == '__main__':
    import sys
    r, nodes, msgs = load(sys.argv[1])
    ids = set(int(x) for x in sys.argv[2:]) if len(sys.argv) > 2 else None
    for n in nodes.values():
        if ids and n.id not in ids: continue
        print(f'#{n.id} type={n.type} parent={n.parent} {n.name} {[(k,v) for k,v in n.attrs.items() if k not in ("name","networkID","typeID","downstreamParentID")]}')
        for d, t, v, a in n.elems:
            ex = {k: v2 for k, v2 in a.items() if k not in ('type', 'description')}
            print(f'   {d:40s} {t:14s} {v!r} {ex if ex else ""}')
        for k, c in n.conditions.items(): print('   COND', k, c.type, [(d, v) for d, t, v, a in c.elems])
        for t, i in n.condsets: print('   CONDSET ->', t, i)
        for k, c in n.common_conditions.items(): print('   COMMONCOND', k, c.type, [(d, v) for d, t, v, a in c.elems])
        for t, i in n.common_condsets: print('   COMMONSET ->', t, i)
