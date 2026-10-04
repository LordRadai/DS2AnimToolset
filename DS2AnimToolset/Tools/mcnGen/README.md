# xml2mcn: rebuild morphemeConnect projects from DS2 network exports

These tools turn a compiled morpheme network export (`<chr>.xml`, the `NetworkDefinition` XML that the
DS2AnimToolset decompiler writes) back into an editable **morphemeConnect 3.6.2** project (`<chr>.mcn`).
Connect itself writes the `.mcn`: the tools generate Lua scripts that rebuild the network through
Connect's scripting API, then patch in the few things Lua cannot create.

Verified on c1020 (2418 nodes, every node type DS2 uses there). c7770 (170 nodes) was verified with an
earlier version of the converter.

---

## Requirements

* Python 3.8+ (standard library only).
* morphemeConnect 3.6.2.
* **Connect must be closed** while a script runs: `morphemeConnect.exe` is single instance, so a second,
  headless copy quits immediately.

## Input folder

Put the decompiler output for one character in a folder:

```
c1020\
  c1020.xml             network export (required)
  NodeIDNamesTable.xml  node names and state names (the only source of names, see below)
  c1020_Library.xml     animation library: AnimIndex -> anim file, take, sync track, format, options
  c1020_Preset.xml
  c1020_0.mrarig        exported rig: joint index -> name, hip / trajectory joints
  c1020_0.mrctrl
  c1020.xmd             character model (moved to model_xmd\ automatically)
  motion_xmd\*.xmd      animations
  morphemeMarkup\*.xml  animation markup
```

The finished Connect project lives in the same folder. `$(RootDir)` is that folder, through the generated
`c1020.mcp`.

---

## Rebuilding a network

### One click: `rebuild.bat`

Edit `tools\rebuild_config.ini`:

```ini
INPUT_XML=E:\Claude\c1020\c1020.xml
CONNECT=C:\Program Files (x86)\NaturalMotion\morphemeConnect 3.6.2\bin\morphemeConnect.exe
PYTHON=python
CP_CONFIG=          ; empty = tools\cp_config.json
CLEAN=1             ; 1 = delete the previous .mcn / paths first
```

Then run `tools\mcnGen.bat`, or `tools\mcnGen.bat other_config.ini` to use another config, for example
one per character. It refuses to start if Connect is open. It runs all five steps below (skipping 3 and 4
when not needed), prints the result of each stage and the diff summary, writes `build\diff_full.txt`, and
runs the layout check.

### Step by step

If a network needs no stage 2 (no ActiveStates and no state pass-down pins), step 1 writes only
`_rebuild.lua`, which exports at the end. Skip steps 3 and 4.

Always start stage 1 from a clean state. Delete `c1020.mcn` and `build\c1020_paths.lua` before re-running it,
because `--inject` and stage 2 rely on the paths that stage 1 recorded.

### What each step prints / writes

The project folder keeps only what the project uses (`.mcn`, `.mcp`, rig, skin, `model_xmd`,
`motion_xmd`, `morphemeMarkup`). Everything that exists only to generate or check the `.mcn` (scripts,
logs, paths, the round-trip export, the diff) goes to `<project>\build\`, which you can delete at any time.

| file | written by | contents |
|---|---|---|
| `<chr>.mcp` | step 1 | project: Z up, metres, anim set `<chr>_0`, rig `$(RootDir)\<chr>_0.mcarig`, sources `motion_xmd`, markup `morphemeMarkup` |
| `build\<chr>_rebuild.lua`, `build\<chr>_stage2.lua` | step 1 | the Connect scripts |
| `<chr>_0.mcarig`, `<chr>_0.mcskin` | stage 1 | rig and skin built from `model_xmd\<chr>.xmd` (hip / trajectory joints from the `.mrarig`); only built if missing |
| `build\<chr>_rebuild.log`, `build\<chr>_stage2.log` | stages | every failed API call (`FAIL` / `NIL`), connect rounds, `DONE failures=N` |
| `build\<chr>_paths.lua` | stages | node id / container -> the path Connect really gave it (duplicate names get `_1` suffixes) |
| `<chr>.mcn` | stages | the project |
| `build\roundtrip\<chr>.xml` | stage 2 | Connect's re-export of the rebuilt project |

Step 1 also lists `UNSUPPORTED` items (node or condition types with no mapping, hierarchy decisions it could
not make) and `UNMAPPED FIELD`s (export fields it does not know). Both should be empty, or at least
understood, before you trust a result.

---

## Control parameter settings

The export only has each CP's default value. `tools\cp_settings.json` adds range, default and
group by CP name. DS2 shares one CP set across characters, so one file serves all of them:

```json
"FB_Speed":     { "type": "float",   "group": "Locomotion", "min": -1.0, "max": 2.0, "default": 0.0 },
"LookAtTarget": { "type": "vector3", "group": "HeadLook",   "min": null, "max": null, "default": [0.0, -10.0, 0.95] }
```

* `min` / `max`: `setRange`. `null` means not set.
* `default`: overrides the export's default (a list for vector CPs).
* `group`: the CP goes into that ControlParameterGroup. Groups are written into the `.mcn` by `--inject`,
  because there is no Lua API for them; a CP belongs to one group only.

```bat
:: add every CP of a network to the file (existing entries are kept)
python tools\xml2mcn.py c1020\c1020.xml --cp-template

:: apply the file to an existing .mcn without rebuilding: writes groups + build\<chr>_cparams.lua
python tools\xml2mcn.py c1020\c1020.xml --cp-only
%CONNECT% -nogui -script E:\Claude\c1020\build\c1020_cparams.lua
```

`--cp-config <file>` uses a different settings file.

Every run (the normal one and `--cp-only`) writes `build\<chr>_cp_config_missing.log`. It lists the control
parameters that have no entry in the settings file, one per line with name, type and export default, so
you can add them (`--cp-template` adds them for you, with `null` ranges).

---

## How names and hierarchy are recovered

Every Connect export name is a path (`SM_Main|BT_BaseAct|SM_BasicMove`), so the path *is* the hierarchy.

* **Names come only from `NodeIDNamesTable.xml`** next to the export; the names inside the export XML are
  ignored. Its `Node` entries give node names (empty = unnamed). Its `StateNode` entries name the
  container holding a state's root node: for a BlendTree state that is the state itself (`...|BT_Idle`,
  which the compiled network has flattened away), for a state machine state it is the owning state
  machine, or the BlendTree wrapping it when the path has one more level. Without the table, the export's
  own names are used as before.
* **Named nodes** keep their exact names. This matters because DS2 looks nodes up
  by name. Node 0 is always `mainNetwork`.
* **When the export has names for every node** (older decompiler output), those paths are used as given.
* **When most nodes are unnamed**, `hierarchy.py` rebuilds the tree from the runtime data:
  * State machine child lists give the states and nested state machines.
  * A node with one consumer lives in its consumer's graph.
  * A multiply connected node lives next to its requester (`downstreamParentID`). If the requester is a
    state machine, the node lives in the state that holds its consumers, which makes that state a
    BlendTree wrapping the inner state machine. A chain of multiply connected nodes sharing one requester
    nests one blend tree per link. In both cases the consumers are reached through one-to-many pass-down
    pins, which is how Connect stores multi-connections.
  * Operators go to the lowest graph above all their consumers.
  * Container names come from named node paths and `StateNode` paths wherever these pass through them
    (`SubAct`, `BT_MoveJump`, `BT_Idle`, ...).
    Otherwise they get generated names: `StateMachine_<id>`, `BlendTree_<root id>_<n>`, `<Type>_<id>`.
  * **AnimWithEvents nodes are named after their animation file.** Transitions are named
    `<source state>_<destination state>`, or `ActiveState_<destination>`.

## Layout

Original editor positions are not in the export, so the layout is generated (`layout.py`):

* **Blend trees**: the Output pin is on the right. Each node's column is its longest path to the output,
  so every input sits left of the node it feeds. Within a column, inputs keep their pin order: the node
  wired to input 1 of a consumer sits above the one wired to input 2 (pin order comes from the node's
  manifest `pinOrder`). The ControlParameters node goes before the last
  (leftmost) node.
* **State machines**: states sit on a grid sized from the largest state, so nothing overlaps. The cell
  assignment is optimised against transition crossings, transitions through other states, and length.
  ActiveStates go in a column on the left.

Headless Connect does not measure nodes, so sizes are estimated (width from the title, height from the
pins). `layoutcheck.py` reports overlaps, inputs not left of their consumer, nodes past the Output pin,
inputs out of pin order, CP node placement, transition crossings, and transitions through states.

---

## Reading the diff

`xmldiff.py` matches every original node to its re-exported counterpart, through `_paths.lua`, and
compares every field. NetworkNodeIds are compared as node names and anim indices as library entries.
The report has these sections:

* `matched N/M nodes`: should be all of them.
* `NAMED NODES WITH CHANGED NAMES`: must be empty.
* `differences by kind`: value differences.
* `fields written by only one side`: fields that DS2's Connect build and vanilla 3.6.2 serialize
  differently. These are not errors. Examples: Blend2 `Loop`/`StartEventIndex`, LockFoot
  `HipIndex`/`KneeIndex`/`FootPivotResistance`, HipsIK ankle indices, TwoBoneIK extras, AnimWithEvents
  `ClipRangeMode`.

Known remaining differences on c1020 (58):

* `DestinationStartSyncEvent`: transitions where DS2 exported 1.0 or 2.0 with every sync flag off. Vanilla
  always writes 0 in that case. The value is unused at runtime.
* Two runtime parents (SM_BasicMove, FilterTransforms_944): matching them would need SM_BasicMove one level
  deeper, which would change its name.
* CP defaults taken from `ds2_control_parameters.json`.

---

## Files

| file | purpose |
|---|---|
| `xml2mcn.py` | the converter: analysis, Lua generation, `--inject`, CP options, project file |
| `nodespecs.py` | per node and condition type: export fields -> Connect attributes and input pins (the inverse of each manifest's `serialize()`) |
| `hierarchy.py` | hierarchy and names for exports without decompiler names |
| `layout.py` | layout computation |
| `manifest_info.py` | reads attribute definitions from Connect's Lua manifests |
| `mcnxml.py` | export XML parser (`python mcnxml.py X.xml [ids...]` dumps decoded nodes) |
| `xmldiff.py` | semantic round-trip diff |
| `layoutcheck.py` | layout rule checker for a saved `.mcn` |
| `ds2_control_parameters.json` | CP ranges / defaults / groups |

### Supported types

Nodes: AnimWithEvents, Blend2, BlendN, FeatherBlend2, SubtractiveBlend, Switch, Sequence, PassThrough,
PlaySpeedModifier, Freeze / LastFramesTransforms, FilterTransforms, MirrorTransforms, SmoothTransforms,
HeadLook, TwoBoneIK, LockFoot, HipsIK, PredictiveUnevenTerrain, EmitRequestOnDiscreteEvent,
OperatorFunction, OperatorOneInputArithmetic / OperatorReRange, OperatorSmoothFloat / Vector3,
OperatorFloatsToVector3, OperatorRandomFloat, state machines, all CP types.

Transitions: Transit, TransitMatchEvents (with ActiveState sources, transitions to self, destination
sub-states).

Conditions: MessageCondition, ControlParamInRange, ControlParamTest, FractionThroughSource, InEventRange,
InDurationEvent, FractionThroughDurationEvent, UserDataEvent, InSubState.

To add a node type, write a `spec_<Type>` in `nodespecs.py`, or use `simple('<ManifestName>')` when the
fields are named like the manifest's attributes, then add it to `SPECS`. Read the manifest's `serialize()`
in `scripts\manifest\...` to see which attribute each field comes from.

## Troubleshooting

* **Nothing happens / the log is not updated**: Connect was still open, or a stale `morphemeConnect.exe` is
  running.
* **`export ok=true result=false`**: some node failed validation, and Connect does not say which. Open the
  `.mcn` in the GUI and export there to see the validation report. Missing manifest patches (like LockFoot)
  are the usual cause.
* **A whole state is missing from `build\roundtrip\`**: Connect drops invalid states silently. Same cause as above.
* **`Too many instructions`**: Connect's Lua aborts long loops. Keep heavy computation in Python.
