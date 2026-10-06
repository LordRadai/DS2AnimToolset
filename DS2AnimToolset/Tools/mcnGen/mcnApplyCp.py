"""
mcnApplyCp.py - apply the CP settings file to built .mcn files directly: no export xml, no Connect.

    python mcnApplyCp.py <file.mcn | folder> [cp_config.json] [--dry-run]

For every control parameter of the .mcn that the settings file lists:
  min / max  float and vector CPs: <Min>/<Max> (float) after the data pin; int CPs: <MinInt>/<MaxInt> after the
             default. A null min or max removes that bound. Bool CPs have no range.
  group      the CP is moved into that control parameter group (groups left empty are removed).
Bounds equal to Connect's own defaults are left out, as Connect does. Defaults are not touched (they come from the game export when the network is built). A folder is searched
recursively for .mcn files. Each file is rewritten only when something changes.
"""
import json, os, sys
import xml.etree.ElementTree as ET

HERE = os.path.dirname(os.path.abspath(__file__))
PTR = 'MorphemeDB.Networks.Network.ControlParameters.ControlParameterArray.'
# bounds Connect leaves out of the file (its defaults); vector CPs always store both
OMITTED = {'float': (0.0, 1.0), 'int': (0, 10), 'uint': (0, 10)}


def number(v, as_int):
    if as_int: return str(int(round(float(v))))
    v = float(v)
    return str(int(v)) if v == int(v) else repr(v)


def cp_type(el):
    t = el.find('DataPinEntry/DataPin/Type')
    return t.text if t is not None and t.text else 'float'


def apply_range(el, cfg):
    """True when the CP's range elements changed."""
    t = cp_type(el)
    if t == 'bool' or cfg.get('type') == 'bool': return False   # bool CPs have no range
    as_int = t in ('int', 'uint')
    tags = ('MinInt', 'MaxInt') if t == 'int' else ('MinUInt', 'MaxUInt') if t == 'uint' else ('Min', 'Max')
    vtype = 'int' if t == 'int' else 'uint' if t == 'uint' else 'float'
    before = [(c.tag, c.text) for c in el]
    for tag in tags:
        for old in el.findall(tag): el.remove(old)
    # int: after the default (as Connect writes it); float / vector: right after the data pin
    anchor = el.find('DefaultInt' if as_int else 'DataPinEntry')
    if anchor is None: anchor = el.find('DataPinEntry')
    pos = list(el).index(anchor) + 1 if anchor is not None else len(el)
    for k, (tag, key) in enumerate(zip(tags, ('min', 'max'))):
        if cfg.get(key) is None: continue
        if t in OMITTED and float(cfg[key]) == OMITTED[t][k]: continue
        e = ET.Element(tag, type=vtype)
        e.text = number(cfg[key], as_int)
        e.tail = anchor.tail if anchor is not None else None   # same indentation as its neighbours
        el.insert(pos, e); pos += 1
    return [(c.tag, c.text) for c in el] != before


def apply_groups(cpnode, arr, config):
    """True when the groups changed."""
    wanted = {}
    for c in arr.findall('ControlParameter'):
        g = (config.get(c.get('name')) or {}).get('group')
        if g: wanted.setdefault(g, []).append(c.get('name'))
    container = cpnode.find('ControlParametersNode')
    if container is None:
        if not wanted: return False
        container = ET.Element('ControlParametersNode', type='nodeContainer')
        cpnode.insert(list(cpnode).index(arr) + 1, container)
    snapshot = lambda: sorted((g.get('name'), sorted(e.text or '' for e in g.iter('elem')))
                              for g in container.findall('ControlParameterGroup'))
    before = snapshot()
    regrouped = {cp for cps in wanted.values() for cp in cps}
    for grp in container.findall('ControlParameterGroup'):
        ga = grp.find('ControlParameterArray')
        if ga is None: continue
        for e in list(ga):
            if e.text and e.text[len(PTR):] in regrouped: ga.remove(e)
        ga.set('size', str(len(ga)))
    for gname, cps in wanted.items():
        grp = next((g for g in container.findall('ControlParameterGroup') if g.get('name') == gname), None)
        if grp is None: grp = ET.SubElement(container, 'ControlParameterGroup', name=gname, type='node')
        ga = grp.find('ControlParameterArray')
        if ga is None: ga = ET.SubElement(grp, 'ControlParameterArray', type='attributeArray', elemType='pointer')
        for cp in cps: ET.SubElement(ga, 'elem', type='pointer').text = PTR + cp
        ga.set('size', str(len(ga)))
    for grp in container.findall('ControlParameterGroup'):
        ga = grp.find('ControlParameterArray')
        if ga is None or len(ga) == 0: container.remove(grp)
    after = snapshot()
    if len(container) == 0: cpnode.remove(container)
    return after != before


def apply(path, config, dry):
    tree = ET.parse(path)
    net = tree.getroot().find('MorphemeDB/Networks/Network')
    cpnode = net.find('ControlParametersNode') if net is not None else None
    arr = cpnode.find('ControlParameterArray') if cpnode is not None else None
    if arr is None:
        print('%s: no control parameters' % path); return False
    cps = arr.findall('ControlParameter')
    ranges = sum(apply_range(c, config[c.get('name')]) for c in cps if c.get('name') in config)
    groups = apply_groups(cpnode, arr, config)
    listed = sum(1 for c in cps if c.get('name') in config)
    print('%s: %d of %d CPs in the settings file, %d ranges changed%s%s' % (
        path, listed, len(cps), ranges, ', groups changed' if groups else '', ' (dry run)' if dry and (ranges or groups) else ''))
    if (ranges or groups) and not dry:
        with open(path, 'wb') as f:
            f.write(b'<?xml version="1.0" encoding="UTF-8"?>\r\n')
            f.write(ET.tostring(tree.getroot(), encoding='utf-8').replace(b'\n', b'\r\n'))
    return bool(ranges or groups)


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    dry = '--dry-run' in sys.argv
    if not args:
        print(__doc__.strip()); return 1
    target = args[0]
    cfg_path = args[1] if len(args) > 1 else os.path.join(HERE, 'cp_config.json')
    config = {k: v for k, v in json.load(open(cfg_path, encoding='utf-8')).items() if not k.startswith('_') and isinstance(v, dict)}
    files = [target] if os.path.isfile(target) else sorted(
        os.path.join(d, f) for d, _, fs in os.walk(target) for f in fs if f.lower().endswith('.mcn'))
    if not files:
        print('no .mcn found in', target); return 1
    changed = sum(apply(f, config, dry) for f in files)
    print('%d of %d .mcn files %s' % (changed, len(files), 'would change' if dry else 'changed'))
    return 0


if __name__ == '__main__':
    sys.exit(main())
