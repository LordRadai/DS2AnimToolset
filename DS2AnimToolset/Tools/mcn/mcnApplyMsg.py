"""
mcnApplyMsg.py - apply a message (request) group settings file to built .mcn files directly: no export xml, no Connect.

    python mcnApplyMsg.py <file.mcn | folder> [msg_config.json] [--dry-run] [--template]

The settings file has the same shape as cp_config.json, with only the group:
    { "Idle": { "group": "Move" }, "Attack_R": { "group": "Attack" }, "SkinCheck": { "group": null } }
For every message of the .mcn that the file lists, the message is moved into that group; a null group takes it out of
any group. Groups left empty are removed; messages not in the file are left alone. A folder is searched recursively.
Each file is rewritten only when something changes.

--template adds every message found in the .mcn files to the settings file (group null) without changing them.

Connect keeps request groups in the network's RequestsNode:
    <RequestGroupArray type="nodeContainer">
      <RequestGroup name="G" type="node">
        <RequestArray type="attributeArray" size="1" elemType="pointer">
          <elem type="pointer">MorphemeDB.Networks.Network.Requests.RequestArray.Idle</elem>
"""
import json, os, sys
import xml.etree.ElementTree as ET

HERE = os.path.dirname(os.path.abspath(__file__))
PTR = 'MorphemeDB.Networks.Network.Requests.RequestArray.'


def requests_node(tree):
    net = tree.getroot().find('MorphemeDB/Networks/Network')
    node = net.find('RequestsNode') if net is not None else None
    arr = node.find('RequestArray') if node is not None else None
    return node, arr


def apply_groups(node, arr, config):
    """True when the groups changed."""
    wanted, ungrouped = {}, set()
    for r in arr.findall('Request'):
        name = r.get('name')
        if name not in config: continue
        g = config[name].get('group')
        if g: wanted.setdefault(g, []).append(name)
        else: ungrouped.add(name)
    container = node.find('RequestGroupArray')
    if container is None:
        if not wanted: return False
        container = ET.Element('RequestGroupArray', type='nodeContainer')
        container.tail = arr.tail
        node.insert(list(node).index(arr) + 1, container)
    snapshot = lambda: sorted((g.get('name'), sorted(e.text or '' for e in g.iter('elem')))
                              for g in container.findall('RequestGroup'))
    before = snapshot()
    moved = {r for rs in wanted.values() for r in rs} | ungrouped
    for grp in container.findall('RequestGroup'):
        ga = grp.find('RequestArray')
        if ga is None: continue
        for e in list(ga):
            if e.text and e.text[len(PTR):] in moved: ga.remove(e)
        ga.set('size', str(len(ga)))
    for gname, names in wanted.items():
        grp = next((g for g in container.findall('RequestGroup') if g.get('name') == gname), None)
        if grp is None: grp = ET.SubElement(container, 'RequestGroup', name=gname, type='node')
        ga = grp.find('RequestArray')
        if ga is None: ga = ET.SubElement(grp, 'RequestArray', type='attributeArray', elemType='pointer')
        for r in names: ET.SubElement(ga, 'elem', type='pointer').text = PTR + r
        ga.set('size', str(len(ga)))
    for grp in container.findall('RequestGroup'):
        ga = grp.find('RequestArray')
        if ga is None or len(ga) == 0: container.remove(grp)
    after = snapshot()
    if len(container) == 0: node.remove(container)
    return after != before


def save(tree, path):
    with open(path, 'wb') as f:
        f.write(b'<?xml version="1.0" encoding="UTF-8"?>\r\n')
        f.write(ET.tostring(tree.getroot(), encoding='utf-8').replace(b'\n', b'\r\n'))


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    dry, template = '--dry-run' in sys.argv, '--template' in sys.argv
    if not args:
        print(__doc__.strip()); return 1
    target = args[0]
    cfg_path = args[1] if len(args) > 1 else os.path.join(HERE, 'msg_config.json')
    raw = json.load(open(cfg_path, encoding='utf-8')) if os.path.exists(cfg_path) else {}
    files = [target] if os.path.isfile(target) else sorted(
        os.path.join(d, f) for d, _, fs in os.walk(target) for f in fs if f.lower().endswith('.mcn'))
    if not files:
        print('no .mcn found in', target); return 1

    if template:
        added = 0
        for p in files:
            node, arr = requests_node(ET.parse(p))
            for r in (arr.findall('Request') if arr is not None else []):
                if r.get('name') not in raw:
                    raw[r.get('name')] = {'group': None}; added += 1
        with open(cfg_path, 'w', encoding='utf-8') as f: json.dump(raw, f, indent=2)
        print('%s now lists %d messages (%d added)' % (cfg_path, len(raw), added))
        return 0

    if not raw:
        print('no settings file:', cfg_path, '(create it with --template)'); return 1
    config = {k: v for k, v in raw.items() if not k.startswith('_') and isinstance(v, dict)}
    changed = 0
    for p in files:
        tree = ET.parse(p)
        node, arr = requests_node(tree)
        if arr is None:
            print('%s: no messages' % p); continue
        listed = sum(1 for r in arr.findall('Request') if r.get('name') in config)
        did = apply_groups(node, arr, config)
        print('%s: %d of %d messages in the settings file%s' % (
            p, listed, len(arr.findall('Request')), (', groups changed' + (' (dry run)' if dry else '')) if did else ''))
        if did:
            changed += 1
            if not dry: save(tree, p)
    print('%d of %d .mcn files %s' % (changed, len(files), 'would change' if dry else 'changed'))
    return 0


if __name__ == '__main__':
    sys.exit(main())
