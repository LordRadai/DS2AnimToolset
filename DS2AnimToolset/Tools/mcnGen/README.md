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
  c1020_0.mrctrl        (one .mrarig / .mrctrl per animation set: c0001 has _0 and _1)
  c1020.xmd             character model (moved to model_xmd\ automatically)
  motion_xmd\*.xmd      animations
  morphemeMarkup\*.xml  animation markup
```

The finished Connect project lives in the same folder. `$(RootDir)` is that folder, through the generated
`c1020.mcp`.

---

## Rebuilding a network

### One click: `mcnGen.bat`

Edit `rebuild_config.ini` next to the script:

```ini
CONNECT=C:\Program Files (x86)\NaturalMotion\morphemeConnect 3.6.2\bin\morphemeConnect.exe
PYTHON=python
CP_CONFIG=          ; empty = cp_config.json next to the script
CLEAN=1             ; 1 = delete the previous .mcn / paths / round-trip export first
NO_WARN=c1021,c2250 ; characters whose warning box is skipped (known differences), for unattended runs
```

Then run:

```bat
mcnGen.bat                                  :: INPUT_XML from rebuild_config.ini
mcnGen.bat E:\Export\c1020\c1020.xml         :: this network
mcnGen.bat E:\Export\c1020\c1020.xml my.ini  :: this network, another config
```

It refuses to start if Connect is open. It runs all five steps below (skipping 3 and 4 when not needed),
prints the result of each stage and the diff summary, writes `build\diff_full.txt`, and runs the layout
check. It exits with an error code if a step fails.

### Every character in a folder: `mcnGenAll.bat`

```bat
mcnGenAll.bat E:\Export            :: every E:\Export\cXXXX\cXXXX.xml
mcnGenAll.bat E:\Export my.ini     :: same, with another config
```

It runs `mcnGen.bat` on each `cXXXX` subfolder (`c` and four digits) that has a `cXXXX.xml`, keeps going
when one fails, and ends with a count of built / failed / skipped characters and the names of the
failed ones.

### CP settings straight into .mcn files: `mcnApplyCp.py`

```bat
python mcnApplyCp.py D:\Projects\FRPG2_64                 :: every .mcn under the folder
python mcnApplyCp.py D:\Projects\FRPG2_64\c0001\c0001.mcn my_cp_config.json --dry-run
```

Only the .mcn and the CP settings file (default `cp_config.json`): no export xml, no Connect. Sets min/max (bounds
equal to Connect's defaults are left out, as Connect does) and groups; defaults stay as built. Files are rewritten only
when something changes.

### CP settings onto built networks: `mcnUpdateCp.bat`

```bat
mcnUpdateCp.bat E:\Export                          :: projects in E:\Export\cXXXX_project (or cXXXX)
mcnUpdateCp.bat E:\Export D:\Projects\FRPG2_64     :: projects moved elsewhere
```

Applies the CP settings file (`CP_CONFIG`, default `cp_config.json`) to every built character without rebuilding:
groups and vector ranges go straight into the .mcn, float/int ranges and defaults are set by Connect
(`build\cXXXX_cparams.lua` in the project), which then saves. The .mcp and the rest of the network are untouched.
Each character's `cXXXX.xml` export must still be in the export folder.

### A clean Connect project: `mcnPack.bat`

```bat
mcnPack.bat E:\Export\c0001                  :: -> E:\Export\c0001_project
mcnPack.bat E:\Export\c0001 D:\Projects\c0001
```

Puts only what morphemeConnect needs into a project folder. What mcnGen generated is moved out of the
character folder: `<chr>.mcn`, `<chr>.mcp`, the `.mcarig` and `.mcskin` of every animation set, and `build\`.
What the decompiler exported is copied and stays: `motion_xmd`, `model_xmd`, `morphemeMarkup` (as do the
export XMLs, `.mrarig` rigs and names table), so the character can be rebuilt without exporting it again.
Paths in the project are `$(RootDir)`-relative, so the folder can live anywhere. It stops if any of those
files is missing.

`mcnGen.bat` runs it after the Connect stages and before the round-trip export (into `<chr>_project`), so
the project is kept even when Connect cannot export it, e.g. a second pass on an upgraded network without the
animations: `motion_xmd`, `model_xmd` and `morphemeMarkup` are copied when present. The round-trip export then runs
on the packed project (`mcnExport.lua`, log in `build\<chr>_export.log`). When the round-trip check finds
differences, or Connect wrote no round-trip export to check, it also pops up a warning message box and
waits for OK before finishing (in `mcnGenAll.bat` the next character starts after you close it).

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
| `<chr>.mcp` | step 1 | project: Z up, metres, one anim set per library set (`<chr>_0`, `<chr>_1`, ...) each with rig `$(RootDir)\<set>.mcarig`, sources `motion_xmd`, markup `morphemeMarkup` |
| `build\<chr>_rebuild.lua`, `build\<chr>_stage2.lua` | step 1 | the Connect scripts |
| `<set>.mcarig`, `<set>.mcskin` | stage 1 | per animation set, rig and skin built from `model_xmd\<chr>.xmd` (hip / trajectory joints from that set's `.mrarig`); only built if missing |
| `build\<chr>_rebuild.log`, `build\<chr>_stage2.log` | stages | every failed API call (`FAIL` / `NIL`), connect rounds, `DONE failures=N` |
| `build\<chr>_paths.lua` | stages | node id / container -> the path Connect really gave it (duplicate names get `_1` suffixes) |
| `<chr>.mcn` | stages | the project |
| `build\roundtrip\<chr>.xml` | mcnExport.lua (after packing) | Connect's re-export of the rebuilt project |

Step 1 also lists `UNSUPPORTED` items (node or condition types with no mapping, hierarchy decisions it could
not make) and `UNMAPPED FIELD`s (export fields it does not know). Both should be empty, or at least
understood, before you trust a result.

---

## Control parameter settings

The export has each CP's default value, and that is always the one used. `cp_config.json` adds range and
group by CP name. DS2 shares one CP set across characters, so one file serves all of them:

```json
"FB_Speed":     { "type": "float",   "group": "Locomotion", "min": -1.0, "max": 2.0 },
"LookAtTarget": { "type": "vector3", "group": "HeadLook",   "min": null, "max": null }
```

* `min` / `max`: `setRange`. `null` means not set.
* `default`: ignored if present; the default always comes from the export.
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
  * Names win over these rules: a nesting level that a named node's or state entry's path has no
    component for is dropped (logged as `nesting ... dropped`), and a nested blend tree that a path does
    name takes that name (`MoveAttack_Ref`).
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
  `HipIndex`/`KneeIndex`/`FootPivotResistance`, HipsIK ankle indices.

Structure, attributes, transitions and conditions match the game exactly, given DS2's manifest changes
(`TransitBase.lua` destination sub states, `Transit.lua` DestinationStartSyncEvent, False condition,
TwoBoneIK flags).

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
| `cp_config.json` | CP ranges / groups |

### Supported types

Nodes: AnimWithEvents, Blend2, BlendN, FeatherBlend2, SubtractiveBlend, Switch, Sequence, PassThrough,
PlaySpeedModifier, Freeze / LastFramesTransforms, FilterTransforms, MirrorTransforms, SmoothTransforms,
HeadLook, TwoBoneIK, LockFoot, HipsIK, PredictiveUnevenTerrain, BasicUnevenTerrain, GunAimIK,
SingleFrame, ScaleToDuration, ApplyGlobalTime, EmitRequestOnDiscreteEvent,
OperatorFunction, OperatorOneInputArithmetic / OperatorReRange, OperatorSmoothFloat / Vector3,
OperatorFloatsToVector3, OperatorRandomFloat, state machines, all CP types.

Animation sets: any number. Per-set export fields (`X_1`, `X_2`, ..., channel lists `Id_<set>_<i>`,
`SmoothingStrengths_<i>_Set_<set>`) are read once per set and written with `setAttribute(..., SETS[k])`;
each spec function only ever sees the set 1 spelling (`nodespecs.SetView`).

Transitions: Transit, TransitMatchEvents (with ActiveState sources, transitions to self, destination
sub-states).

Conditions: MessageCondition, ControlParamInRange, ControlParamTest, FractionThroughSource, InEventRange,
InDurationEvent, FractionThroughDurationEvent, UserDataEvent, InSubState, False (DS2's always-false
condition, id 607; needs `manifest\conditions\False.lua` in Connect).

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
