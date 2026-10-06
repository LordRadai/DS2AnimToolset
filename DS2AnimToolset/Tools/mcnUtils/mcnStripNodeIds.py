#!/usr/bin/env python3
"""
Remove the node-ID numbers mcnGen bakes into names in morphemeConnect .mcn
files and rename things the way morpheme itself does.

Nodes (<BlendTreeNode>/<StateMachineNode>) named "<Kind>_<id>" or
"<Kind>_<id>_<n>", where <Kind> is the node's <NodeType> (or, for a state
container, the type of its GraphEntry: BlendTree / StateMachine):
    PlaySpeedModifier_8530 -> PlaySpeedModifier
    BlendTree_8504_0       -> BlendTree

Duplicates inside one container follow morpheme's convention: the first keeps
the bare name, the others get NameN (N = how many siblings already use the
name), with an "_" before N when the name already ends in a digit:
    PassThrough, PassThrough1, PassThrough2
    Blend2, Blend2_1, Blend2_2

Names derived from nodes are regenerated from the final node names, but only
when they currently carry an mcnGen ID:
    PassDownPin    In_<id'd node>  -> In_<name of the node feeding the pin>
                   (a pin fed by an outer container's pin inherits its name)
    FlowEdge       <src>_<pin>_To_<dst>_<pin>   (rebuilt from From/To)
    TransitionEdge <src state>_<dst state>      (rebuilt from From/To)

Everything else is untouched: names the user/NMB gave (SM_Guard, BT_Die,
StateMachine_Main), animation nodes (a36_43_0011_..._R_01), control
parameters, requests, conditions, anim sets.

Every pointer (<X type="pointer">MorphemeDB....</X>) through a renamed element
is rewritten, and each output is re-parsed to check all pointers still resolve
to exactly one element.

Usage:
  python mcn_strip_node_ids.py <folder> [--recursive] [--dry-run [-v]]
                               [--out DIR | --in-place]

Default output: <folder>\\stripped\\<file>.mcn (originals untouched).
--in-place overwrites each file after saving <file>.mcn.bak.
"""
import argparse, collections, os, re, sys
import xml.parsers.expat as expat
from xml.sax.saxutils import unescape

NODE_TAGS = {"BlendTreeNode", "StateMachineNode"}
SUFFIX_RE = re.compile(r"^(?P<base>.+?)_(?P<id>\d+(?:_\d+)?)$")
POINTER_RE = re.compile(rb'(type="pointer">)([^<]*)(<)')


class Elem:
    __slots__ = ("tag", "name", "type", "offset", "parent", "node_type",
                 "graph_kind", "ptrs", "seg", "_path")

    def __init__(self, tag, attrs, offset, parent):
        self.tag = tag
        self.name = attrs.get("name")
        self.type = attrs.get("type")
        self.offset = offset
        self.parent = parent
        self.node_type = None
        self.graph_kind = None
        self.ptrs = {}
        self.seg = self.name if self.name is not None else tag
        self._path = None

    def path(self):
        if self._path is None:
            pp = self.parent.path() if self.parent is not None else ()
            # the NaturalMotion document root is not part of pointer paths
            self._path = pp + (self.seg,) if self.parent is not None else ()
        return self._path


def parse(data):
    stack, named = [], []
    texts = {}
    p = expat.ParserCreate("UTF-8")
    p.buffer_text = True

    def start(tag, attrs):
        parent = stack[-1] if stack else None
        e = Elem(tag, attrs, p.CurrentByteIndex, parent)
        if parent is not None and parent.tag == "GraphEntry" and parent.parent is not None \
                and parent.parent.graph_kind is None:
            parent.parent.graph_kind = tag
        stack.append(e)
        if e.name is not None:
            named.append(e)

    def end(tag):
        e = stack.pop()
        t = texts.pop(id(e), None)
        if t is not None and stack:
            if tag == "NodeType":
                stack[-1].node_type = "".join(t)
            else:  # pointer
                stack[-1].ptrs[tag] = "".join(t)

    def chars(t):
        if stack and (stack[-1].tag == "NodeType" or stack[-1].type == "pointer"):
            texts.setdefault(id(stack[-1]), []).append(t)

    p.StartElementHandler, p.EndElementHandler, p.CharacterDataHandler = start, end, chars
    p.Parse(data, True)
    return named


def ptr_tuple(s):
    return tuple(unescape(x) for x in s.split("."))


def dedupe(base, taken):
    if base not in taken:
        return base
    sep = "_" if base[-1].isdigit() else ""
    n = 1
    while "%s%s%d" % (base, sep, n) in taken:
        n += 1
    return "%s%s%d" % (base, sep, n)


def plan_renames(named):
    """Returns {elem: new_name}."""
    by_path = {e.path(): e for e in named}
    sibs = collections.defaultdict(list)
    for e in named:
        if e.type == "node":
            sibs[id(e.parent)].append(e)

    renames = {}

    def final(e):
        return renames.get(e, e.name)

    def assign(candidates_by_container):
        """candidates: [(elem, wanted)] per container, in document order."""
        for key, cands in candidates_by_container.items():
            moving = {e for e, _ in cands}
            taken = {e.name for e in sibs[key] if e not in moving}
            taken |= {renames[e] for e in sibs[key] if e in renames and e not in moving}
            for e, wanted in cands:
                new = dedupe(wanted, taken)
                taken.add(new)
                if new != e.name:
                    renames[e] = new

    # mcnGen node kinds seen in this file -> to spot IDs in derived names
    kinds = set()

    # 1. nodes
    cands = collections.defaultdict(list)
    for e in named:
        if e.tag in NODE_TAGS and e.type == "node":
            kind = e.node_type or e.graph_kind
            m = SUFFIX_RE.match(e.name)
            if kind and m and m.group("base") == kind:
                cands[id(e.parent)].append((e, kind))
                kinds.add(kind)
    assign(cands)
    if not kinds:
        return renames
    id_token = re.compile(r"(?:^|_)(?:%s)_\d+" % "|".join(map(re.escape, sorted(kinds, key=len, reverse=True))))

    def resolve(ptr):
        return by_path.get(ptr_tuple(ptr))

    def endpoint(ptr):
        """FlowEdge endpoint label in mcnGen's style, from FINAL names."""
        t = ptr_tuple(ptr)
        pin = resolve(ptr)
        if len(t) >= 3 and t[-2] == "Pins":
            node = by_path.get(t[:-2])
            return "%s_%s" % (final(node) if node else t[-3], final(pin) if pin else t[-1])
        if len(t) >= 4 and t[-2] == "DataPinEntry":  # control parameter
            cp = by_path.get(t[:-2])
            return "ControlParameters%s_%s" % (final(cp) if cp else t[-3], t[-1])
        return "%s_%s" % (t[-2], t[-1])  # e.g. Network.Result

    # 2. pass-down pins: In_<source node>
    feeders = {}
    for e in named:
        if e.tag == "FlowEdge" and "From" in e.ptrs and "To" in e.ptrs:
            feeders.setdefault(ptr_tuple(e.ptrs["To"]), e.ptrs["From"])
    def needs_pin_rename(e):
        return e.tag == "PassDownPin" and e.name.startswith("In_") and bool(id_token.search(e.name[3:]))

    wanted_pin = {}

    def pin_wanted(e, depth=0):
        """A pass-down pin is named after the node feeding it; when it is fed by
        an outer container's pass-down pin, it inherits that pin's name."""
        if not needs_pin_rename(e):
            return e.name
        if e in wanted_pin:
            return wanted_pin[e]
        want = None
        src = feeders.get(e.path())
        if src and depth < 64:
            f = resolve(src)
            t = ptr_tuple(src)
            if f is not None and f.tag == "PassDownPin" and f is not e:
                want = pin_wanted(f, depth + 1)
            elif len(t) >= 3 and t[-2] == "Pins" and by_path.get(t[:-2]) is not None:
                want = "In_" + final(by_path[t[:-2]])
        if want is None:
            m = SUFFIX_RE.match(e.name[3:])
            want = "In_" + (m.group("base") if m and m.group("base") in kinds else e.name[3:])
        wanted_pin[e] = want
        return want

    cands = collections.defaultdict(list)
    for e in named:
        if needs_pin_rename(e):
            cands[id(e.parent)].append((e, pin_wanted(e)))
    assign(cands)

    # 3. edges
    cands = collections.defaultdict(list)
    for e in named:
        if e.tag not in ("FlowEdge", "TransitionEdge") or not id_token.search(e.name):
            continue
        f, t = e.ptrs.get("From"), e.ptrs.get("To")
        if not f or not t:
            continue
        if e.tag == "FlowEdge":
            want = "%s_To_%s" % (endpoint(f), endpoint(t))
        else:
            fe, te = resolve(f), resolve(t)
            if fe is None or te is None:
                continue
            want = "%s_%s" % (final(fe), final(te))
        cands[id(e.parent)].append((e, want))
    assign(cands)
    return renames


def rewrite(data, renames):
    patches = []
    for e, new in renames.items():
        i = data.index(b' name="', e.offset) + 7
        j = data.index(b'"', i)
        patches.append((i, j, new.encode("utf-8")))
    patches.sort()
    out, last = [], 0
    for i, j, b in patches:
        out.append(data[last:i]); out.append(b); last = j
    out.append(data[last:])
    data = b"".join(out)

    prefix = {e.path(): new for e, new in renames.items()}
    changed = [0]

    def fix(m):
        segs = m.group(2).decode("utf-8").split(".")
        orig = [unescape(s) for s in segs]
        dirty = False
        for k in range(1, len(orig) + 1):
            new = prefix.get(tuple(orig[:k]))
            if new is not None:
                segs[k - 1] = new
                dirty = True
        if not dirty:
            return m.group(0)
        changed[0] += 1
        return m.group(1) + ".".join(segs).encode("utf-8") + m.group(3)

    return POINTER_RE.sub(fix, data), changed[0]


def check_pointers(data):
    paths = collections.Counter(e.path() for e in parse(data))
    unresolved = ambiguous = 0
    for m in POINTER_RE.finditer(data):
        n = paths.get(ptr_tuple(m.group(2).decode("utf-8")), 0)
        unresolved += n == 0
        ambiguous += n > 1
    dup_siblings = sum(v - 1 for v in paths.values() if v > 1)
    return unresolved, ambiguous, dup_siblings


def process(path, args):
    with open(path, "rb") as f:
        data = f.read()
    renames = plan_renames(parse(data))
    by_tag = collections.Counter(e.tag for e in renames)
    print("%s: %d renamed  (%s)" % (path, len(renames),
          ", ".join("%s=%d" % kv for kv in by_tag.most_common())))
    if args.verbose:
        for e, new in sorted(renames.items(), key=lambda kv: kv[0].offset):
            print("   %-16s %s -> %s" % (e.tag, e.name, new))
    if args.dry_run or not renames:
        return
    before = check_pointers(data)
    out, nptr = rewrite(data, renames)
    after = check_pointers(out)
    print("   pointers rewritten: %d   unresolved/ambiguous/duplicate-names before %s, after %s"
          % (nptr, before, after))
    if any(a > b for a, b in zip(after, before)):
        print("   WARNING: output has more broken pointers or duplicate names than the input")

    if args.in_place:
        dst, bak = path, path + ".bak"
        if not os.path.exists(bak):
            os.replace(path, bak)
    else:
        dst = os.path.join(args.out or os.path.join(args.folder, "stripped"),
                           os.path.relpath(path, args.folder))
        os.makedirs(os.path.dirname(dst), exist_ok=True)
    with open(dst + ".tmp", "wb") as f:
        f.write(out)
    os.replace(dst + ".tmp", dst)
    print("   wrote", dst)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("folder")
    ap.add_argument("--recursive", "-r", action="store_true")
    ap.add_argument("--dry-run", "-n", action="store_true", help="list renames, write nothing")
    ap.add_argument("--verbose", "-v", action="store_true", help="print every rename")
    g = ap.add_mutually_exclusive_group()
    g.add_argument("--out", help="output folder (default <folder>/stripped)")
    g.add_argument("--in-place", action="store_true", help="overwrite, keeping .mcn.bak")
    args = ap.parse_args()
    args.folder = os.path.abspath(args.folder)
    if args.out:
        args.out = os.path.abspath(args.out)
    skip = os.path.normcase(args.out or os.path.join(args.folder, "stripped"))

    files = []
    for root, dirs, fnames in os.walk(args.folder):
        dirs[:] = [d for d in dirs if args.recursive
                   and os.path.normcase(os.path.join(root, d)) != skip]
        files += [os.path.join(root, n) for n in fnames if n.lower().endswith(".mcn")]
    if not files:
        sys.exit("no .mcn files in " + args.folder)
    for f in sorted(files):
        try:
            process(f, args)
        except Exception as ex:
            print("%s: FAILED: %r" % (f, ex))


if __name__ == "__main__":
    main()
