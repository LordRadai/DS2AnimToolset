# Converts a morpheme XMD (ANIM_CYCLE, per-frame quat/translate/scale) to a Dark Souls 2 Ap2Ani file.
# (Ap2AnimFileSingle, one animation, key type 8, all vectors in the float table).
# The file layout is the same for 32 bit and 64 bit:
# all links are u32 offsets from the file start, object stride 0x40, same key types.
# Target platforms for the output file:
#   --target le64  64 bit little-endian
#   --target le32  32 bit little-endian
#   --target be32  32 bit big-endian
import argparse, math, re, struct, sys

E = '<'  # byte order for every struct call, set from the command line

def parse(path):
    text = open(path, encoding='latin-1').read().replace('\r', '')
    joints = {}
    for m in re.finditer(r'^JOINT (\S+)\n\{(.*?)\n\}', text, re.S | re.M):
        body = m.group(2)
        g = lambda k: [float(x) for x in re.search(r'^\t' + k + r'\s+(.*)$', body, re.M).group(1).split()]
        jid = int(re.search(r'^\tid (\d+)', body, re.M).group(1))
        parent = int(re.search(r'^\tparent (\d+)', body, re.M).group(1))
        joints[jid] = dict(name=m.group(1), t=g('translate'), r=g('rotation'), s=g('scale'), parent=parent)
    anim = {}
    for m in re.finditer(r'BONE_ANIM (\d+)\n\t\{\n(.*?)\n\t\}', text, re.S):
        rows = [[float(x) for x in l.split()] for l in m.group(2).split('\n') if l.strip()]
        anim[int(m.group(1))] = rows
    return joints, anim

def quat_to_euler_xyz(q):
    x, y, z, w = q
    ex = math.atan2(2 * (w * x + y * z), 1 - 2 * (x * x + y * y))
    ey = math.asin(max(-1.0, min(1.0, 2 * (w * y - z * x))))
    ez = math.atan2(2 * (w * z + x * y), 1 - 2 * (y * y + z * z))
    return [ex, ey, ez]

# Z-up (morpheme) -> Y-up (game), facing the game's forward:
# rotate -90 degrees about X, (x, y, z) -> (x, z, -y), then 180 degrees about Y, (x, y, z) -> (-x, y, -z).
# Combined: (x, y, z) -> (-x, z, y), quaternion Ry(180) * Rx(-90) = (0, sqrt(0.5), sqrt(0.5), 0).
# Children are in parent space, so only the top bones need it.
UP_Q = [0.0, math.sqrt(0.5), math.sqrt(0.5), 0.0]

def up_vec(v):
    return [-v[0], v[2], v[1]]

def quat_mul(a, b):
    ax, ay, az, aw = a; bx, by, bz, bw = b
    return [aw * bx + ax * bw + ay * bz - az * by,
            aw * by - ax * bz + ay * bw + az * bx,
            aw * bz + ax * by - ay * bx + az * bw,
            aw * bw - ax * bx - ay * by - az * bz]

def main(src, dst, up_bones=None):
    joints, anim = parse(src)
    ids = sorted(anim)
    if up_bones is None:  # default: bones whose parent is not in the file
        up_bones = {joints[j]['name'] for j in ids if j in joints and joints[j]['parent'] not in joints}
    print('Z-up -> Y-up on:', ', '.join(sorted(up_bones)))
    for jid in ids:
        if jid in joints and joints[jid]['name'] in up_bones:
            j = joints[jid]
            j['t'] = up_vec(j['t']); j['r'] = quat_mul(UP_Q, j['r'])
            anim[jid] = [quat_mul(UP_Q, r[0:4]) + up_vec(r[4:7]) + r[7:10] for r in anim[jid]]
    nframes = len(anim[ids[0]])
    vecs, vindex = [], {}
    def vec(v):
        k = tuple(struct.pack(E + '3f', *v))
        if k not in vindex:
            vindex[k] = len(vecs); vecs.append(v)
        return vindex[k]

    objs = []
    for jid in ids:
        rows = anim[jid]
        q = []
        for r in rows:
            qq = r[0:4]
            n = math.sqrt(sum(c * c for c in qq)) or 1.0
            qq = [c / n for c in qq]
            if qq[3] < 0: qq = [-c for c in qq]          # game rebuilds w >= 0
            q.append(qq)
        t = [r[4:7] for r in rows]
        s = [r[7:10] for r in rows]
        keys = []
        for i in range(nframes):
            a, b = t[max(i - 1, 0)], t[min(i + 1, nframes - 1)]
            d = (min(i + 1, nframes - 1) - max(i - 1, 0)) or 1
            tan = [(b[k] - a[k]) / d for k in range(3)]   # per-frame derivative (Hermite is scaled by key spacing)
            ti = vec(tan)
            keys.append((i, vec(t[i]), ti, ti, vec(q[i][0:3]), vec(s[i])))
        j = joints.get(jid, dict(name='bone%d' % jid, t=[0, 0, 0], r=[0, 0, 0, 1], s=[1, 1, 1]))
        objs.append((j, keys))

    # layout: header 0x40 | objects 0x40 each | TOD blocks 0xC each | keys | vectors | names
    HDR = 0x40
    off_obj = HDR
    off_tod = off_obj + 0x40 * len(objs)
    off_keys = off_tod + 0xC * len(objs)
    off_vec = off_keys + sum(len(k) * 24 for _, k in objs)
    off_names = off_vec + 12 * len(vecs)

    names = b''; name_off = []
    for j, _ in objs:
        name_off.append(off_names + len(names)); names += j['name'].encode('ascii') + b'\0'

    out = bytearray()
    out += struct.pack(E + 'IiiIIIIiiII', 0x20100910, 0, nframes, off_obj, len(objs), off_vec, 0, len(vecs), 0, 0, 0)
    out += bytes(7)                      # 0x2C: all vector IDs use the float table
    out += bytes(HDR - len(out))
    ko = off_keys
    for i, (j, keys) in enumerate(objs):
        o = bytearray(0x40)
        struct.pack_into(E + 'I', o, 0x00, name_off[i])
        struct.pack_into(E + 'h', o, 0x08, 1)          # animated
        struct.pack_into(E + 'H', o, 0x12, 0xFFFF)     # rotation animated flag
        struct.pack_into(E + '3f', o, 0x14, *j['t'])
        struct.pack_into(E + '3f', o, 0x20, *quat_to_euler_xyz(j['r']))
        struct.pack_into(E + '3f', o, 0x2C, *j['s'])
        struct.pack_into(E + 'I', o, 0x38, off_tod + 0xC * i)
        out += o
    for j, keys in objs:
        out += struct.pack(E + 'IiI', ko, len(keys), 8); ko += 24 * len(keys)
    for _, keys in objs:
        for k in keys: out += struct.pack(E + '6i', *k)
    for v in vecs: out += struct.pack(E + '3f', *v)
    out += names
    out += bytes((-len(out)) % 16)
    open(dst, 'wb').write(out)
    print(f'{len(objs)} objects, {nframes} frames, {len(vecs)} vectors, {len(out)} bytes')

ap = argparse.ArgumentParser(description='Convert a morpheme XMD to an FRPG2 .ani')
ap.add_argument('input'); ap.add_argument('output')
ap.add_argument('--target', choices=['le64', 'le32', 'be32'], default='le64',
                help='le64 = PC FRPG2_64 (default), le32 = little-endian 32-bit, be32 = PS3')
ap.add_argument('--up-bones', help='comma-separated bones to convert Z-up -> Y-up (default: top bones)')
ap.add_argument('--no-up', action='store_true', help='skip the Z-up -> Y-up conversion (raw morpheme axes)')
args = ap.parse_args()
E = '>' if args.target == 'be32' else '<'
main(args.input, args.output, set() if args.no_up else (set(args.up_bones.split(',')) if args.up_bones else None))
