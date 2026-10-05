"""
layout.py - automatic layout, computed in Python and emitted as plain position calls (Connect's Lua
interpreter aborts long-running scripts with "Too many instructions").

  blend trees : Output pin on the right; each item's column is its longest path to the output, so every
                input sits left of the item it feeds; the ControlParameters node goes before the last
                (leftmost) column.
  state machines: states on a grid (cell = largest state + margin); the assignment of states to cells is
                optimised to minimise transition crossings, transitions running through states and
                transition length.  ActiveStates sit in a column on the left.
Node sizes are estimated (headless Connect does not measure nodes): width from the title, height from pins.
"""
import math

COL_GAP, ROW_GAP = 80, 40


def node_size(leaf, npins):
    return max(210, 8 * len(leaf) + 70), 40 + 20 * max(npins, 2)


def blend_tree(items, edges, size, root=None):
    """items: ids; edges: (src, dst, input slot) inside the graph; size(item) -> (w, h).
    Returns ({item: (x, y)}, output (x, y), cp (x, y))."""
    consumers = {i: [] for i in items}
    slot = {}
    for s, d, k in edges:
        if s in consumers and d in consumers and s != d:
            consumers[s].append(d)
            slot[(s, d)] = min(k, slot.get((s, d), k))
    col = {}
    def depth(i, guard=()):
        if i in col: return col[i]
        if i in guard: return 1
        d = 1 + max([depth(c, guard + (i,)) for c in consumers[i]] or [0])
        col[i] = d
        return d
    for i in items: depth(i)
    maxcol = max(col.values()) if col else 1
    colw = {}
    for i in items: colw[col[i]] = max(colw.get(col[i], 0), size(i)[0])
    colx, x = {}, 0
    for c in range(1, maxcol + 1):
        x -= colw.get(c, 210) + COL_GAP
        colx[c] = x
    pos, ypos = {}, {}
    for c in range(1, maxcol + 1):
        members = [i for i in items if col[i] == c]
        # top-down: by the consumer's row, then by input slot (input 1 above input 2)
        members.sort(key=lambda i: min([(ypos.get(d, 0), slot[(i, d)]) for d in consumers[i]] or [(0, 0)]) + (str(i),))
        y = 0
        for i in members:
            pos[i] = (colx[c], y)
            ypos[i] = y
            y += size(i)[1] + ROW_GAP
    return pos, (0, 0), (colx.get(maxcol, -210) - 200 - COL_GAP, 0)


def _orient(a, b, c):
    v = (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0])
    return (v > 0) - (v < 0)


def _cross(p1, p2, q1, q2):
    return _orient(p1, p2, q1) * _orient(p1, p2, q2) < 0 and _orient(q1, q2, p1) * _orient(q1, q2, p2) < 0


def _hits(a, b, c, hw, hh):
    for s in range(1, 12):
        x, y = a[0] + (b[0] - a[0]) * s / 12, a[1] + (b[1] - a[1]) * s / 12
        if abs(x - c[0]) < hw and abs(y - c[1]) < hh: return True
    return False


def state_machine(states, actives, transitions, size, budget=40000):
    """states/actives: ids; transitions: (src, dst).  Returns {id: (x, y)}.
    States go on a grid, ActiveStates in a column left of it; both the grid cells of the states and the order of the
    ActiveStates in their column are optimised against crossings, transitions through states and length."""
    n = len(states)
    pos = {}
    na = len(actives)
    ah = max([size(a)[1] for a in actives] or [0]) + ROW_GAP           # ActiveState slot pitch
    aslot = {a: k for k, a in enumerate(actives)}
    def acentre(a):
        w, h = size(a)
        return (-w / 2 - 2 * COL_GAP, aslot[a] * ah + h / 2)
    def place_actives():
        for a in actives:
            w, h = size(a)
            pos[a] = (-w - 2 * COL_GAP, aslot[a] * ah)
    if n == 0:
        place_actives(); return pos
    maxw = max(size(s)[0] for s in states); maxh = max(size(s)[1] for s in states)
    cw, ch = maxw + 2 * COL_GAP, maxh + 3 * ROW_GAP
    cols = int(math.ceil(math.sqrt(n)))
    rows = int(math.ceil(n / cols))
    if n > 2 and cols * rows == n: rows += 1
    ncells = cols * rows
    idx = {s: i for i, s in enumerate(states)}
    edges = [(a, b) for a, b in transitions if a != b and (a in idx or a in aslot) and (b in idx or b in aslot)]
    cell = list(range(n))
    def centre(e):
        if e in aslot: return acentre(e)
        k = cell[idx[e]]
        return ((k % cols) * cw + maxw / 2, (k // cols) * ch + maxh / 2)
    def order_actives():
        # barycentre: each ActiveState level with the states it leads to
        def bary(a):
            ys = [centre(d if s == a else s)[1] for s, d in edges if a in (s, d) and (d if s == a else s) in idx]
            return sum(ys) / len(ys) if ys else 0.0
        for k, a in enumerate(sorted(actives, key=lambda a: (bary(a), aslot[a]))): aslot[a] = k
    def cost():
        segs = [(centre(a), centre(b)) for a, b in edges]
        c = 0.0
        for i, (p1, p2) in enumerate(segs):
            c += math.hypot(p2[0] - p1[0], p2[1] - p1[1]) / (cw + ch)
            for j in range(i + 1, len(segs)):
                if {edges[i][0], edges[i][1]} & {edges[j][0], edges[j][1]}: continue
                if _cross(p1, p2, segs[j][0], segs[j][1]): c += 100
            for s in states:
                if s in edges[i]: continue
                if _hits(p1, p2, centre(s), maxw / 2, maxh / 2): c += 60
        return c
    if edges and (n > 2 or na > 1):
        order_actives()
        m = len(edges)
        budget = min(budget, max(300, 4000000 // (m * m + m * (n + na) + 1)))
        best, evals, improved = cost(), 0, True
        while improved and evals < budget:
            improved = False
            for i in range(n if n > 2 else 0):
                for target in range(ncells):
                    if cell[i] == target: continue
                    other = cell.index(target) if target in cell else None
                    old = cell[i]
                    cell[i] = target
                    if other is not None: cell[other] = old
                    c = cost(); evals += 1
                    if c < best - 1e-9: best, improved = c, True
                    else:
                        cell[i] = old
                        if other is not None: cell[other] = target
                    if evals >= budget: break
                if evals >= budget: break
            # ActiveStates: swap slots in their column
            for i in range(na):
                for j in range(i + 1, na):
                    if evals >= budget: break
                    a, b = actives[i], actives[j]
                    aslot[a], aslot[b] = aslot[b], aslot[a]
                    c = cost(); evals += 1
                    if c < best - 1e-9: best, improved = c, True
                    else: aslot[a], aslot[b] = aslot[b], aslot[a]
    place_actives()
    for s in states:
        k = cell[idx[s]]
        pos[s] = ((k % cols) * cw, (k // cols) * ch)
    return pos
