"""
Per node type inversion of Connect's manifest serialize() functions.

spec(ctx, node) -> (manifest type, attrs, inputs)
  attrs  : [(attribute, value, per_anim_set)]  value is a python value; ('#path', p) for a path
           reference, ('#request', id) for a request, ('#cp', id) for a control parameter
  inputs : [(field, pin)]  export fields that hold a source node id -> Connect input pin name

ctx provides: .manifest (manifest_info), .joints {index: name}, .nchannels, .unmapped(set)
"""
import math, re

DUR_FLAGS = ('DurationEventBlendPassThrough', 'DurationEventBlendSameUserData',
             'DurationEventBlendOnOverlap', 'DurationEventBlendWithinRange')
IGNORE_ALWAYS = {'NumAnimSets', 'WorldUpAxisX', 'WorldUpAxisY', 'WorldUpAxisZ', 'UpAxisIndex',
                 'AssumeSimpleHierarchy', 'NodeEmitsMessages', 'NumMessageSlots', 'SourceNodeCount',
                 'ConnectedPinCount', 'ClipRangeMode_1'}
SOURCE_FIELDS = {'InputNodeID': 'Source', 'SourceNodeID': 'Source', 'NodeConnectedTo': 'Source'}


def pin_for(field):
    if field in SOURCE_FIELDS: return SOURCE_FIELDS[field]
    m = re.fullmatch(r'Source(\d+)NodeID', field)
    if m: return 'Source' + m.group(1)
    m = re.fullmatch(r'ConnectedNodeID_(\d+)', field)
    if m: return 'Source' + m.group(1)
    if field == 'RotationDeltaEulerNodeID' or field == 'RotationDeltaQuatNodeID': return 'RotationDelta'
    return field


def conv(v, atype):
    if atype in ('bool',): return bool(v)
    if atype in ('int',): return int(v) if not isinstance(v, bytes) else 0
    if atype in ('float',): return float(v)
    return v


def generic(ctx, node, mtype, skip=(), rename=None):
    """Fields named like attributes ('X' or per-set 'X_1'), joint indices 'YIndex_1' -> 'YName'."""
    rename = rename or {}
    attrs, inputs = [], []
    mattrs = ctx.manifest.get(mtype, {}).get('attrs', {})
    for d, t, v, a in node.elems:
        if d in skip or d in IGNORE_ALWAYS: continue
        if t == 'NetworkNodeId':
            if v not in (0xFFFFFFFF, -1): inputs.append((d, pin_for(d)))
            continue
        if d in rename:
            attrs.append((rename[d], v, False)); continue
        base, per = (d[:-2], True) if d.endswith('_1') else (d, False)
        if base.endswith('Index') and base[:-5] + 'Name' in mattrs:
            nm = base[:-5] + 'Name'
            if v in ctx.joints: attrs.append((nm, ctx.joints[v], True))
            elif v not in (0xFFFFFFFF, -1): ctx.unmapped.add('%s.%s joint %r' % (mtype, d, v))
            continue
        if base in mattrs:
            ad = mattrs[base]
            attrs.append((base, conv(v, ad['type']), ad['perAnimSet'] or per))
        else:
            ctx.unmapped.add('%s.%s' % (mtype, d))
    return attrs, inputs


# node types whose numbered fields are not per animation set (blend weights, CP defaults, SM children, transits)
NONSET_TYPES = {9, 10, 20, 21, 22, 23, 24, 25, 108, 131, 400, 402, 403}


def set_field(node, d, K, descs):
    """(name as animation set 1 would spell it, set index) for a per-set field of a K-set export, else None."""
    if node.type in NONSET_TYPES: return None
    m = re.fullmatch(r'Id_(\d+)_(\d+)', d)                    # channel lists: Id_<set>_<i>
    if m and node.type in (105, 135):
        return 'Id_1_' + m.group(2), int(m.group(1))
    m = re.fullmatch(r'(.+_Set_)(\d+)', d)                     # SmoothingStrengths_<i>_Set_<set>
    if m: return m.group(1) + '1', int(m.group(2))
    m = re.fullmatch(r'(.+)_(\d+)', d)
    if m:
        base, k = m.group(1), int(m.group(2))
        if 1 <= k <= K and all('%s_%d' % (base, j) in descs for j in range(1, K + 1)) and '%s_%d' % (base, K + 1) not in descs:
            return base + '_1', k
    return None


class SetView:
    """The node as animation set k sees it: set k's fields renamed to the set 1 spelling, other sets' dropped,
    so the single-set spec functions work unchanged."""
    def __init__(self, node, k, K):
        self._node = node
        descs = {d for d, t, v, a in node.elems}
        count = {}
        for d, t, v, a in node.elems: count[d] = count.get(d, 0) + 1
        seen = {}
        self.elems = []
        for d, t, v, a in node.elems:
            sf = set_field(node, d, K, descs)
            if sf is None:
                # a plain field written once per set (MirrorTransforms EventOffset): the k-th copy is set k's
                seen[d] = seen.get(d, 0) + 1
                if count[d] == K and node.type not in NONSET_TYPES and seen[d] != k: continue
                self.elems.append((d, t, v, a))
            elif sf[1] == k: self.elems.append((sf[0], t, v, a))
    def __getattr__(self, name): return getattr(self._node, name)
    def get(self, desc, default=None, nth=0):
        i = 0
        for d, t, v, a in self.elems:
            if d == desc:
                if i == nth: return v
                i += 1
        return default


def dur_events(node, attrs):
    for f in DUR_FLAGS:
        if node.get(f) is not None: attrs.append((f, bool(node.get(f)), False))
    if node.get('DurationEventBlendInSequence') is not None:
        attrs.append(('DurationEventBlendIgnoreEventOrder', not node.get('DurationEventBlendInSequence'), False))


DUR_SKIP = DUR_FLAGS + ('DurationEventBlendInSequence',)


def blend2(ctx, node, mtype='Blend2'):
    attrs, inputs = generic(ctx, node, mtype, skip=DUR_SKIP + ('BlendWeight_0', 'BlendWeight_1', 'BlendMode') +
                            tuple(d for d, *_ in node.elems if d.startswith('ChannelAlphasSet')))
    if mtype != 'SubtractiveBlend':
        attrs.append(('BlendWeights', [float(node.get('BlendWeight_0', 0.0)), float(node.get('BlendWeight_1', 1.0))], False))
    if node.get('BlendMode') is not None:
        bm = int(node.get('BlendMode'))
        attrs += [('RotationBlendMode', 1 if bm >= 2 else 0, False), ('PositionBlendMode', bm % 2, False)]
    dur_events(node, attrs)
    if node.get('ChannelAlphasSet0Count') is not None:
        n = int(node.get('ChannelAlphasSet0Count'))
        attrs.append(('ChannelAlphas', [float(node.get('ChannelAlphasSet0_Value%d' % i, 0.0)) for i in range(n)], True))
    return attrs, inputs


def blendn(ctx, node, mtype):
    count = 0
    while node.get('Source%dNodeID' % count) is not None: count += 1
    skip = DUR_SKIP + ('WrapWeight',) + tuple('SourceWeight_%d' % i for i in range(count + 1))
    attrs, inputs = generic(ctx, node, mtype, skip=skip)
    w = [float(node.get('SourceWeight_%d' % i, 0.0)) for i in range(count)]
    if node.get('WrapWeights'): w.append(float(node.get('WrapWeight', 0.0)))
    attrs.append(('SourceWeightDistribution', 0, False))   # 0 = custom: keep the exact weights
    attrs.append(('SourceWeights', w, False))
    dur_events(node, attrs)
    return attrs, inputs


def channel_mask(ctx, node, count_field, id_fmt, attr, invert_listed=True):
    n = int(node.get(count_field, 0) or 0)
    listed = {int(node.get(id_fmt % (i + 1))) for i in range(n)}
    return (attr, [not (i in listed) for i in range(ctx.nchannels)], True)


def spec_AnimWithEvents(ctx, node):
    attrs = []
    idx = node.get('AnimIndex')
    take = ctx.anim_take(idx)
    if take: attrs.append(('AnimationTake', take, True))
    else: ctx.unmapped.add('AnimWithEvents anim index %r not in library' % (idx,))
    for a in ('Loop', 'PlayBackwards', 'GenerateAnimationDeltas', 'PreComputeSyncEventTracks'):
        attrs.append((a, bool(node.get(a, False)), False))
    attrs += [('DefaultClip', bool(node.get('DefaultClip_1', True)), True),
              ('ClipStartFraction', float(node.get('ClipStartFraction_1', 0.0)), True),
              ('ClipEndFraction', float(node.get('ClipEndFraction_1', 1.0)), True),
              ('StartEventIndex', int(node.get('StartEventIndex_1', 0)), True)]
    return 'AnimWithEvents', attrs, []


def spec_HeadLook(ctx, node):
    attrs, inputs = generic(ctx, node, 'HeadLook', skip=('EndEffectorOffsetX_1', 'EndEffectorOffsetY_1', 'EndEffectorOffsetZ_1',
                                                         'PointingVectorX_1', 'PointingVectorY_1', 'PointingVectorZ_1', 'Bias_1'))
    for a in ('PointingVectorX', 'PointingVectorY', 'PointingVectorZ', 'Bias', 'EndEffectorOffsetX', 'EndEffectorOffsetY', 'EndEffectorOffsetZ'):
        if node.get(a + '_1') is not None: attrs.append((a, float(node.get(a + '_1')), True))
    return 'HeadLook', attrs, inputs


def spec_TwoBoneIK(ctx, node):
    ref = ['MidJointReferenceAxis%s_1' % c for c in 'XYZ']
    attrs, inputs = generic(ctx, node, 'TwoBoneIK', skip=tuple(ref) + ('MidJointIndex_1', 'RootJointIndex_1'))
    vals = [float(node.get(f, 0.0)) for f in ref]
    use = any(abs(v) > 0 for v in vals)
    attrs.append(('UseReferenceAxis', use, True))
    if use:
        for c, v in zip('XYZ', vals): attrs.append(('MidJointReferenceAxis' + c, v, True))
    return 'TwoBoneIK', attrs, inputs


def spec_PredictiveUnevenTerrain(ctx, node, mtype='PredictiveUnevenTerrain'):
    # hip and knee are derived from the ankle by Connect, so only ankle / ball / toe names are attributes
    skip = tuple('%s%sIndex_1' % (s, j) for s in ('Left', 'Right') for j in ('Hip', 'Knee'))
    attrs, inputs = generic(ctx, node, mtype, skip=skip)
    for s in ('Left', 'Right'):
        if node.get('%sBallIndex_1' % s) not in (None, 0xFFFFFFFF): attrs.append(('BallJointEnable', True, True))
        if node.get('%sToeIndex_1' % s) not in (None, 0xFFFFFFFF): attrs.append(('ToeJointEnable', True, True))
    return mtype, attrs, inputs


def spec_BasicUnevenTerrain(ctx, node):
    return spec_PredictiveUnevenTerrain(ctx, node, 'BasicUnevenTerrain')


def spec_HipsIK(ctx, node):
    attrs, inputs = generic(ctx, node, 'HipsIK')
    # DS2's HipsIK export has no ankle indices; Connect's validate wants the ankle to be the ball's parent
    for side in ('Left', 'Right'):
        ball = node.get('%sBallIndex_1' % side)
        if node.get('%sAnkleIndex_1' % side) is None and ball in ctx.joint_parent and ctx.joint_parent[ball] in ctx.joints:
            attrs.append(('%sAnkleName' % side, ctx.joints[ctx.joint_parent[ball]], True))
    if node.get('RotationDeltaQuatNodeID') is not None: attrs.append(('InputRotationType', 0, False))
    elif node.get('RotationDeltaEulerNodeID') is not None: attrs.append(('InputRotationType', 1, False))
    return 'HipsIK', attrs, inputs


def spec_LockFoot(ctx, node):
    attrs, inputs = generic(ctx, node, 'LockFoot', skip=('HipIndex_1', 'KneeIndex_1'))
    return 'LockFoot', attrs, inputs


def spec_FilterTransforms(ctx, node):
    attrs, inputs = generic(ctx, node, 'FilterTransforms',
                            skip=tuple(d for d, *_ in node.elems if d.startswith('Id_') or d.startswith('FilterIdCount')))
    attrs.append(channel_mask(ctx, node, 'FilterIdCount_1', 'Id_1_%d', 'ChannelIsOutput'))
    return 'FilterTransforms', attrs, inputs


def spec_MirrorTransforms(ctx, node):
    attrs, inputs = generic(ctx, node, 'MirrorTransforms',
                            skip=tuple(d for d, *_ in node.elems if d.startswith('Id_') or d.startswith('NonMirroredIdCount')))
    attrs.append(channel_mask(ctx, node, 'NonMirroredIdCount_1', 'Id_1_%d', 'MirrorChannels'))
    return 'MirrorTransforms', attrs, inputs


def spec_SmoothTransforms(ctx, node):
    attrs, inputs = generic(ctx, node, 'SmoothTransforms',
                            skip=tuple(d for d, *_ in node.elems if d.startswith('SmoothingStrengths') or d.startswith('numSmoothing')))
    n = int(node.get('numSmoothingStrengthsSet_1', 0) or 0)
    attrs.append(('ChannelSmoothingStrengths', [float(node.get('SmoothingStrengths_%d_Set_1' % (i + 1), 0.0)) for i in range(n)], True))
    return 'SmoothTransforms', attrs, inputs


def spec_EmitRequestOnDiscreteEvent(ctx, node):
    attrs, inputs = [], []
    if node.get('SourceNodeID') not in (None, 0xFFFFFFFF): inputs.append(('SourceNodeID', 'Source'))
    k = 0
    while node.get('ActionID_%d' % k) is not None:
        act = {1: 'Set', 2: 'Clear', 3: 'Clear All'}.get(int(node.get('ActionID_%d' % k)), 'Set')
        attrs.append(('Action%d' % k, act, False))
        if node.get('EmittedMessageID_%d' % k) is not None:
            attrs.append(('EmittedRequest%d' % k, ('#request', int(node.get('EmittedMessageID_%d' % k))), False))
        if node.get('EventUserData_%d' % k) is not None:
            attrs.append(('EventUserData%d' % k, int(node.get('EventUserData_%d' % k)), False))
        if node.get('TargetNodePath_%d' % k):
            attrs.append(('Target%d' % k, ('#path', node.get('TargetNodePath_%d' % k)), False))
        k += 1
    return 'EmitRequestOnDiscreteEvent', attrs, inputs


OPS_FUNC = ['sin', 'cos', 'tan', 'exp', 'log', 'sqrt', 'abs', 'asin', 'acos']
OPS_ARITH = ['*', '+', '/', '-', 'min', 'max', 'emult']


def spec_112(ctx, node):
    code = int(node.get('OperationCode', 0))
    inputs = [('Input', 'Input')] if node.get('Input') not in (None, 0xFFFFFFFF) else []
    if code == 6 and node.get('ConstantValueY') is None and node.get('ConstantValueX') not in (None, 0.0):
        # OperatorReRange: times/add -> pick input range 0..1
        times, add = float(node.get('ConstantValue')), float(node.get('ConstantValueX'))
        return 'OperatorReRange', [('InputRange1', 0.0, False), ('InputRange2', 1.0, False),
                                   ('OutputRange1', add, False), ('OutputRange2', add + times, False)], inputs
    return 'OperatorOneInputArithmetic', [('Operation', OPS_ARITH[code] if 0 <= code < len(OPS_ARITH) else '*', False),
                                          ('ConstantValue', float(node.get('ConstantValue', 0.0)), False)], inputs


def spec_110(ctx, node):
    code = int(node.get('OperationCode', 0))
    inputs = [('Input', 'Input')] if node.get('Input') not in (None, 0xFFFFFFFF) else []
    return 'OperatorFunction', [('Operation', OPS_FUNC[code] if 0 <= code < len(OPS_FUNC) else 'sin', False)], inputs


def spec_142(ctx, node):
    scalar = bool(node.get('IsScalar', True))
    mtype = 'OperatorSmoothFloat' if scalar else 'OperatorSmoothVector3'
    attrs = [('SmoothTime', float(node.get('SmoothTime', 0.0)), False),
             ('SmoothVelocity', bool(node.get('SmoothVelocity', False)), False),
             ('UseInitValueOnInit', bool(node.get('UseInitValueOnInit', False)), False)]
    for c in ('X',) if scalar else ('X', 'Y', 'Z'):
        v = node.get('InitValue' + c, node.get('InitValue_' + c))   # decompiler writes InitValue_X
        if v is not None: attrs.append(('InitValue' + c, float(v), False))
    inputs = [('Input', 'Input')] if node.get('Input') not in (None, 0xFFFFFFFF) else []
    return mtype, attrs, inputs


def spec_146(ctx, node):
    seed, interval = int(node.get('Seed', 0)), float(node.get('Interval', 0.0))
    attrs = [('Min', float(node.get('Min', 0.0)), False), ('Max', float(node.get('Max', 1.0)), False)]
    attrs += [('GenerateSeed', 'User Specified', False), ('Seed', seed, False)] if seed else [('GenerateSeed', 'Every Activation', False)]
    attrs += [('DurationMode', 'Specify', False), ('Interval', interval, False)] if interval > 0 else [('DurationMode', 'Every Update', False)]
    return 'OperatorRandomFloat', attrs, []


def spec_126(ctx, node):
    return ('Freeze' if node.get('passThroughTransformsOnce', True) else 'LastFramesTransforms'), [], []


def simple(mtype, **kw):
    def f(ctx, node):
        if mtype in ('Blend2', 'FeatherBlend2', 'SubtractiveBlend'):
            a, i = blend2(ctx, node, mtype)
        elif mtype in ('BlendN', 'Switch'):
            a, i = blendn(ctx, node, mtype)
        else:
            a, i = generic(ctx, node, mtype, **kw)
        return mtype, a, i
    return f


SPECS = {
    104: spec_AnimWithEvents, 122: spec_HeadLook, 120: spec_TwoBoneIK, 121: spec_LockFoot, 129: spec_HipsIK,
    138: spec_PredictiveUnevenTerrain, 105: spec_FilterTransforms, 135: spec_MirrorTransforms,
    500: spec_SmoothTransforms, 153: spec_EmitRequestOnDiscreteEvent, 110: spec_110, 112: spec_112,
    142: spec_142, 146: spec_146, 126: spec_126,
    107: simple('Blend2'), 114: simple('FeatherBlend2'), 170: simple('SubtractiveBlend'),
    108: simple('BlendN'), 131: simple('Switch'), 134: simple('PassThrough'), 125: simple('PlaySpeedModifier'),
    133: simple('Sequence'), 144: simple('OperatorFloatsToVector3'),
    109: simple('SingleFrame'), 119: simple('ApplyGlobalTime'), 151: simple('ScaleToDuration'),
    150: simple('GunAimIK', skip=('WorldUpAxisX', 'WorldUpAxisY', 'WorldUpAxisZ')), 136: spec_BasicUnevenTerrain,
}


# ---------------------------------------------------------------- conditions
def cond_spec(ctx, c):
    """-> (manifest type, attrs) for a transition condition"""
    t, g = c.type, c.get
    if t == 601:
        return 'MessageCondition', [('Message', ('#request', g('MessageID')), False), ('OnNotSet', bool(g('OnNotSet')), False)]
    if t == 602:
        return 'UserDataEvent', [('EventUserTypeID', int(g('EventUserTypeID')), False)]
    if t == 603:
        return 'FractionThroughSource', [('TriggerPercent', float(g('TestFraction')), False)]
    if t == 609:
        return 'ControlParamInRange', [('ControlParameter', ('#cp', g('RuntimeNodeID')), False),
                                       ('LowerTestValue', float(g('LowerTestValue')), False),
                                       ('UpperTestValue', float(g('UpperTestValue')), False),
                                       ('NotInRange', bool(g('NotInRange')), False)]
    if t == 611:
        return 'InEventRange', [('EventRangeStart', float(g('EventRangeStart')), False), ('EventRangeEnd', float(g('EventRangeEnd')), False)]
    if t == 616:
        cmp = ('<' if g('LessThanOperation') else '>') + ('=' if g('OrEqual') else '')
        return 'ControlParamTest', [('ControlParameter', ('#cp', g('RuntimeNodeID')), False),
                                    ('TriggerValue', float(g('TestValue')), False), ('Comparison', cmp, False)]
    if t == 617:
        return 'InSubState', [('Node', ('#node', g('NodeID')), False)]
    if t == 607:
        return 'False', []   # TRANSCOND_FALSE: needs the DS2 False.lua condition manifest
    if t in (610, 618):
        mtype = 'FractionThroughDurationEvent' if t == 610 else 'InDurationEvent'
        attrs = [(d, (bool(v) if ty == 'bool' else float(v) if ty == 'float' else int(v)), False) for d, ty, v, a in c.elems]
        return mtype, attrs
    return None, []
