"""Read node/condition/transition attribute definitions from morphemeConnect's Lua manifests."""
import glob, os, re

MANIFEST_DIR = r'manifest'


def _attributes_block(body):
    i = body.find('attributes =')
    if i < 0: return ''
    j = body.find('{', i)
    depth, k = 0, j
    while k < len(body):
        if body[k] == '{': depth += 1
        elif body[k] == '}':
            depth -= 1
            if depth == 0: return body[j:k + 1]
        k += 1
    return body[j:]


def _entries(block):
    """Top-level { ... } entries of the attributes table."""
    out, depth, start = [], 0, None
    for k, ch in enumerate(block):
        if ch == '{':
            depth += 1
            if depth == 2: start = k
        elif ch == '}':
            if depth == 2 and start is not None: out.append(block[start:k + 1])
            depth -= 1
    return out


def load(manifest_dir=MANIFEST_DIR):
    """name -> {'kind', 'file', 'id', 'attrs': {attr: {'type', 'perAnimSet', 'value'}}}"""
    info = {}
    for f in glob.glob(os.path.join(manifest_dir, '**', '*.lua'), recursive=True):
        s = open(f, encoding='latin1').read()
        for m in re.finditer(r'register(Node|StateMachineNode|Condition|Transition|PhysicsNode)\(\s*"(\w+)"', s):
            kind, name = m.groups()
            body = s[m.end():]
            nxt = re.search(r'\nregister(Node|StateMachineNode|Condition|Transition|PhysicsNode)\(', body)
            body = body[:nxt.start()] if nxt else body
            attrs = {}
            for e in _entries(_attributes_block(body)):
                nm = re.search(r'name\s*=\s*"(\w+)"', e)
                if not nm: continue
                ty = re.search(r'type\s*=\s*"(\w+)"', e)
                pas = re.search(r'perAnimSet\s*=\s*(true|false)', e)
                val = re.search(r'value\s*=\s*([^,\n}]+)', e)
                attrs[nm.group(1)] = {'type': ty.group(1) if ty else None,
                                      'perAnimSet': bool(pas and pas.group(1) == 'true') or (ty and ty.group(1) == 'animationTake'),
                                      'value': val.group(1).strip() if val else None}
            ids = [int(x) for x in re.findall(r'generateNamespacedId\(idNamespaces\.NaturalMotion,\s*(\d+)\)', body[:3000])]
            po = re.search(r'pinOrder\s*=\s*\{([^}]*)\}', body)
            pin_order = re.findall(r'"(\w+)"', po.group(1)) if po else []
            info[name] = {'kind': kind, 'file': f, 'ids': ids, 'attrs': attrs, 'pinOrder': pin_order}
    return info


if __name__ == '__main__':
    import sys
    info = load()
    for n in sys.argv[1:]:
        i = info[n]
        print(n, i['kind'], i['ids'])
        for a, d in i['attrs'].items(): print('   ', a, d)
