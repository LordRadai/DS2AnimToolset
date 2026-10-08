"""Batch-recreate morphemeConnect 3.6.2 .mcskin files from model XMDs.

Each project folder looks like:

    <root>\\c1240\\c1240.mcn
    <root>\\c1240\\model_xmd\\c1240.xmd
    <root>\\c1240\\motion_xmd\\*.xmd

and gets <root>\\c1240\\c1240_0.mcskin (next to the .mcn, named <folder>_0).

The .mcskin embeds its own copy of the mesh and skin weights, so it has to be
rebuilt whenever the model XMD is re-exported. The project's .mcarig is never
touched (it carries the trajectory/hip tags, mirror mappings and joint limits).

How it works:
  1. Python scans <root> (or a single project folder) and writes _mcskin_batch.lua.
  2. Connect runs it headless: `morphemeConnect.exe -nogui -script _mcskin_batch.lua`.
     For each project the script tries anim.createSkin(xmd, tmp.mcskin, scale);
     if that fails it falls back to anim.createRig(xmd, tmp.mcarig, traj, hip),
     which writes tmp.mcskin next to the throwaway tmp.mcarig.
     Everything goes to <root>\\_mcskin_tmp, never into the project.
  3. Python backs up each existing <folder>_0.mcskin (.bak, or .bak.<timestamp>
     if a .bak exists) and copies the new skin into place.

Connect is single instance: close any running Connect first, or the headless
one quits immediately.

Settings come from config.ini (the same file rebuild.bat uses: key=value lines,
';' comments). Looked up next to this script unless --config is given. Keys used:
  CONNECT   path to morphemeConnect.exe
Command-line options override the config.

Usage:
  python mcskin_batch.py <root>                    # generate, run Connect, install
  python mcskin_batch.py <root> --only c1240 c1370 # subset of folders
  python mcskin_batch.py <root> --lua-only         # just write the Lua, print the command
  python mcskin_batch.py <root> --install-only     # install from an earlier run's tmp dir

createRig/createSkin refuse an XMD with NaN skin weights and write nothing;
those folders are reported as failed and the batch carries on.
"""

import argparse
import datetime
import os
import shutil
import subprocess
import sys
from pathlib import Path

DEFAULT_CONNECT_DIR = Path(r"C:\Program Files (x86)\NaturalMotion\morphemeConnect 3.6.2")
TMP_DIR_NAME = "_mcskin_tmp"
LUA_NAME = "_mcskin_batch.lua"
RESULTS_NAME = "results.txt"
CONFIG_NAME = "config.ini"


def read_config(path):
    """key=value lines, ';' or '#' comments, no sections (rebuild.bat's config.ini)."""
    cfg = {}
    for raw in path.read_text(encoding="utf-8-sig", errors="replace").splitlines():
        line = raw.strip()
        if not line or line[0] in ";#" or "=" not in line:
            continue
        key, value = line.split("=", 1)
        cfg[key.strip().upper()] = value.strip().strip('"')
    return cfg


def find_connect_exe(given):
    if given:
        exe = Path(given)
        if exe.is_dir():
            exe = exe / "morphemeConnect.exe"
        return exe if exe.is_file() else None
    for cand in (DEFAULT_CONNECT_DIR / "morphemeConnect.exe",
                 DEFAULT_CONNECT_DIR / "bin" / "morphemeConnect.exe"):
        if cand.is_file():
            return cand
    hits = list(DEFAULT_CONNECT_DIR.rglob("morphemeConnect.exe")) if DEFAULT_CONNECT_DIR.is_dir() else []
    return hits[0] if hits else None


def find_model_xmd(folder):
    model_dir = folder / "model_xmd"
    if not model_dir.is_dir():
        return None, "no model_xmd folder"
    xmds = sorted(p for p in model_dir.iterdir() if p.suffix.lower() == ".xmd")
    if not xmds:
        return None, "model_xmd has no .xmd"
    for p in xmds:
        if p.stem.lower() == folder.name.lower():
            return p, None
    if len(xmds) == 1:
        return xmds[0], None
    return None, "model_xmd has %d .xmd files and none is named %s.xmd" % (len(xmds), folder.name)


def find_mcn(folder):
    mcns = sorted(p for p in folder.iterdir() if p.suffix.lower() == ".mcn")
    for p in mcns:
        if p.stem.lower() == folder.name.lower():
            return p
    return mcns[0] if mcns else None


def collect_projects(root, only):
    """Returns (jobs, skipped). root may itself be a project folder."""
    if find_mcn(root) is not None or (root / "model_xmd").is_dir():
        folders = [root]
    else:
        folders = sorted(p for p in root.iterdir() if p.is_dir() and p.name != TMP_DIR_NAME)
    if only:
        wanted = {o.lower() for o in only}
        folders = [f for f in folders if f.name.lower() in wanted]

    jobs, skipped = [], []
    for folder in folders:
        mcn = find_mcn(folder)
        if mcn is None:
            if (folder / "model_xmd").is_dir():
                skipped.append((folder.name, "no .mcn in folder"))
            continue  # not a Connect project, ignore silently
        xmd, err = find_model_xmd(folder)
        if err:
            skipped.append((folder.name, err))
            continue
        jobs.append({
            "name": folder.name,
            "xmd": xmd,
            "dest": mcn.parent / (folder.name + "_0.mcskin"),
        })
    return jobs, skipped


def lua_str(s):
    s = str(s).replace("\\", "/")
    return '"' + s.replace('"', '\\"') + '"'


def write_lua(jobs, tmp_root, lua_path, scale, traj, hip):
    results = tmp_root / RESULTS_NAME
    lines = [
        "-- generated by mcskin_batch.py; Lua 5.0 (no '#', no '...')",
        "local jobs = {",
    ]
    for j in jobs:
        lines.append("  { name = %s, xmd = %s, out = %s }," % (
            lua_str(j["name"]), lua_str(j["xmd"]), lua_str(tmp_root / j["name"])))
    lines += [
        "}",
        "local scale = %r" % float(scale),
        "local traj = %s" % lua_str(traj),
        "local hip = %s" % lua_str(hip),
        "local log = io.open(%s, \"w\")" % lua_str(results),
        "",
        "local exists = function(path)",
        "  local f = io.open(path, \"rb\")",
        "  if f then io.close(f) return true end",
        "  return false",
        "end",
        "",
        "local remove = function(path) if exists(path) then os.remove(path) end end",
        "",
        "for i = 1, table.getn(jobs) do",
        "  local j = jobs[i]",
        "  local skin = j.out .. \"/\" .. j.name .. \"_0.mcskin\"",
        "  local rig = j.out .. \"/\" .. j.name .. \"_0.mcarig\"",
        "  remove(skin)",
        "  remove(rig)",
        "  local method = \"createSkin\"",
        "  local okCall, ok = pcall(anim.createSkin, j.xmd, skin, scale)",
        "  if not (okCall and ok and exists(skin)) then",
        "    remove(skin)",
        "    method = \"createRig\"",
        "    okCall, ok = pcall(anim.createRig, j.xmd, rig, traj, hip)",
        "  end",
        "  local status = \"FAIL\"",
        "  if okCall and ok and exists(skin) then status = \"OK\" end",
        "  local detail = method",
        "  if not okCall then detail = method .. \": \" .. tostring(ok) end",
        "  log:write(j.name .. \"\\t\" .. status .. \"\\t\" .. detail .. \"\\n\")",
        "  log:flush()",
        "end",
        "log:close()",
    ]
    lua_path.write_text("\n".join(lines) + "\n", encoding="utf-8")
    return results


def read_results(results_path):
    out = {}
    if not results_path.is_file():
        return out
    for line in results_path.read_text(encoding="utf-8", errors="replace").splitlines():
        parts = line.split("\t")
        if len(parts) >= 2:
            out[parts[0]] = (parts[1], parts[2] if len(parts) > 2 else "")
    return out


def backup(path):
    bak = path.with_name(path.name + ".bak")
    if bak.exists():
        stamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
        bak = path.with_name(path.name + ".bak." + stamp)
    shutil.copy2(path, bak)
    return bak


def install(jobs, tmp_root, results):
    ok, failed = [], []
    for j in jobs:
        status, detail = results.get(j["name"], ("MISSING", "Connect did not reach this project"))
        new_skin = tmp_root / j["name"] / (j["name"] + "_0.mcskin")
        if status != "OK" or not new_skin.is_file():
            failed.append((j["name"], "%s %s (check %s for NaN weights)" % (status, detail, j["xmd"].name)))
            continue
        bak = backup(j["dest"]) if j["dest"].exists() else None
        shutil.copy2(new_skin, j["dest"])
        ok.append((j["name"], detail, bak))
    return ok, failed


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("root", help="folder of Connect project folders, or one project folder")
    ap.add_argument("--only", nargs="+", help="only these project folder names")
    ap.add_argument("--config", help="config.ini to read (default: config.ini next to this script)")
    ap.add_argument("--connect", help="path to morphemeConnect.exe (or its folder); overrides CONNECT in config.ini")
    ap.add_argument("--scale", type=float, default=1.0, help="rig unit scale factor for createSkin (default 1.0 = metres)")
    ap.add_argument("--traj", default="Master", help="trajectory joint for the createRig fallback (default Master)")
    ap.add_argument("--hip", default="Root", help="hip joint for the createRig fallback (default Root)")
    ap.add_argument("--lua-only", action="store_true", help="write the Lua and print the Connect command, don't run it")
    ap.add_argument("--install-only", action="store_true", help="skip Connect, install skins from an earlier run")
    ap.add_argument("--keep-tmp", action="store_true", help="keep the _mcskin_tmp folder after installing")
    args = ap.parse_args()

    config_path = Path(args.config) if args.config else Path(__file__).resolve().parent / CONFIG_NAME
    if config_path.is_file():
        cfg = read_config(config_path)
        print("config %s" % config_path)
    elif args.config:
        sys.exit("config not found: %s" % config_path)
    else:
        cfg = {}
    connect = args.connect or cfg.get("CONNECT") or None

    root = Path(args.root).resolve()
    if not root.is_dir():
        sys.exit("not a folder: %s" % root)

    jobs, skipped = collect_projects(root, args.only)
    for name, why in skipped:
        print("SKIP %-12s %s" % (name, why))
    if not jobs:
        sys.exit("no projects with model_xmd found under %s" % root)

    tmp_root = (root.parent if len(jobs) == 1 and jobs[0]["dest"].parent == root else root) / TMP_DIR_NAME
    lua_path = tmp_root / LUA_NAME
    results_path = tmp_root / RESULTS_NAME

    if not args.install_only:
        if tmp_root.exists():
            shutil.rmtree(tmp_root)
        for j in jobs:
            (tmp_root / j["name"]).mkdir(parents=True, exist_ok=True)
        write_lua(jobs, tmp_root, lua_path, args.scale, args.traj, args.hip)
        print("wrote %s (%d projects)" % (lua_path, len(jobs)))

        exe = find_connect_exe(connect)
        cmd = [str(exe) if exe else "morphemeConnect.exe", "-nogui", "-script", str(lua_path)]
        if args.lua_only:
            print("close Connect, then run:\n  " + subprocess.list2cmdline(cmd))
            print("then: python %s %s --install-only" % (Path(__file__).name, args.root))
            return
        if exe is None:
            sys.exit("morphemeConnect.exe not found%s; set CONNECT in config.ini or pass --connect"
                     % ((" at " + connect) if connect else ""))
        print("running Connect headless (close any open Connect first)...")
        subprocess.run(cmd, cwd=str(exe.parent))

    results = read_results(results_path)
    if not results:
        sys.exit("no results in %s: did Connect run the script? (a running Connect makes the headless one quit)" % results_path)

    ok, failed = install(jobs, tmp_root, results)
    for name, method, bak in ok:
        print("OK   %-12s %s%s" % (name, method, ("  (backup %s)" % bak.name) if bak else ""))
    for name, why in failed:
        print("FAIL %-12s %s" % (name, why))
    print("%d recreated, %d failed, %d skipped" % (len(ok), len(failed), len(skipped)))

    if not failed and not args.keep_tmp:
        shutil.rmtree(tmp_root, ignore_errors=True)
    elif failed:
        print("kept %s for inspection" % tmp_root)


if __name__ == "__main__":
    main()
