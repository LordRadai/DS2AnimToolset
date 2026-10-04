"""layoutcheck.py - verify the layout rules on a saved .mcn: overlaps, inputs left of consumers,
everything left of the Output pin, ControlParameters node left of the first CP reader."""
import sys, re, collections, xml.etree.ElementTree as ET

def f(e, tag, d=0.0):
    x = e.find(tag)
    return float(x.text) if x is not None and x.text else d

def box(n):
    """Connect only stores Width/Height once a node has been drawn: estimate like layout.py does."""
    w, h = f(n, 'Width', 0), f(n, 'Height', 0)
    if w <= 0:
        pins = n.find('Pins')
        w = max(210, 8 * len(n.get('name')) + 70)
        h = 40 + 20 * (len(pins) if pins is not None else 2)
    return (f(n, 'XPosition'), f(n, 'YPosition'), w, h)

def overlaps(boxes):
    n = 0
    for i in range(len(boxes)):
        for j in range(i + 1, len(boxes)):
            a, b = boxes[i], boxes[j]
            if a[0] < b[0] + b[2] and b[0] < a[0] + a[2] and a[1] < b[1] + b[3] and b[1] < a[1] + a[3]: n += 1
    return n

def _o(a, b, c):
    v = (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0])
    return (v > 0) - (v < 0)

def seg_cross(p1, p2, q1, q2):
    return _o(p1, p2, q1) * _o(p1, p2, q2) < 0 and _o(q1, q2, p1) * _o(q1, q2, p2) < 0

def seg_box(a, b, bx):
    for s in range(1, 12):
        x, y = a[0] + (b[0] - a[0]) * s / 12, a[1] + (b[1] - a[1]) * s / 12
        if bx[0] < x < bx[0] + bx[2] and bx[1] < y < bx[1] + bx[3]: return True
    return False

def main(path):
    t = ET.parse(path)
    stats = collections.Counter()
    for g in t.iter():
        if g.tag == 'BlendTree' and g.find('BlendTreeNodes') is not None:
            nodes = {n.get('name'): n for n in g.find('BlendTreeNodes')}
            boxes = {k: box(n) for k, n in nodes.items()}
            stats['blend trees'] += 1
            stats['blend tree overlaps'] += overlaps(list(boxes.values()))
            ox = f(g, 'OutputPinXPosition')
            stats['nodes right of output'] += sum(1 for b in boxes.values() if b[0] + b[2] > ox)
            cpx = g.find('ControlParamXPosition')
            readers = []
            for fe in g.iter('FlowEdge'):
                fr, to = fe.findtext('From') or '', fe.findtext('To') or ''
                m1 = re.search(r'BlendTreeNodes\.([^.]+)\.Pins\.[^.]+$', fr)
                m2 = re.search(r'BlendTreeNodes\.([^.]+)\.Pins\.[^.]+$', to)
                if m1 and m2 and m1.group(1) in boxes and m2.group(1) in boxes and to.startswith(fr.split('.BlendTreeNodes.')[0]):
                    stats['edges'] += 1
                    if boxes[m1.group(1)][0] >= boxes[m2.group(1)][0]: stats['inputs not left of consumer'] += 1
                if 'ControlParameters.ControlParameterArray' in fr and m2 and m2.group(1) in boxes: readers.append(boxes[m2.group(1)][0])
            if readers and cpx is not None:
                stats['graphs reading CPs'] += 1
                if float(cpx.text) >= min(b[0] for b in boxes.values()): stats['CP node not left of the last node'] += 1
        if g.tag == 'StateMachine' and g.find('StateMachineNodes') is not None:
            sts = [n for n in g.find('StateMachineNodes')]
            stats['state machines'] += 1
            stats['state overlaps'] += overlaps([box(n) for n in sts])
            bx = {n.get('name'): box(n) for n in sts}
            segs = []
            for te in g.iter('TransitionEdge'):
                a = (te.findtext('From') or '').split('.')[-1]; b = (te.findtext('To') or '').split('.')[-1]
                if a in bx and b in bx and a != b:
                    ca = (bx[a][0] + bx[a][2] / 2, bx[a][1] + bx[a][3] / 2); cb = (bx[b][0] + bx[b][2] / 2, bx[b][1] + bx[b][3] / 2)
                    segs.append((a, b, ca, cb))
            for i in range(len(segs)):
                for j in range(i + 1, len(segs)):
                    if {segs[i][0], segs[i][1]} & {segs[j][0], segs[j][1]}: continue
                    if seg_cross(segs[i][2], segs[i][3], segs[j][2], segs[j][3]): stats['transition crossings'] += 1
                for k, b in bx.items():
                    if k in segs[i][:2]: continue
                    if seg_box(segs[i][2], segs[i][3], b): stats['transitions through a state'] += 1
    for k, v in sorted(stats.items()): print('%-36s %d' % (k, v))

if __name__ == '__main__':
    main(sys.argv[1])
