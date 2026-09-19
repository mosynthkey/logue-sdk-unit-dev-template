#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
DEST="${1:-$ROOT/dist}"
PLUGIN_JSON="$ROOT/plugin.json"

if [[ ! -f "$PLUGIN_JSON" ]]; then
  echo "missing $PLUGIN_JSON" >&2
  exit 1
fi

PLUGIN_ID="$(python3 -c 'import json,sys; print(json.load(open(sys.argv[1]))["id"])' "$PLUGIN_JSON")"

rm -rf "$DEST"
mkdir -p "$DEST/units" "$DEST/sim"
cp "$ROOT/web/index.html" "$ROOT/web/nts1-midi.js" "$ROOT/web/coi-serviceworker.js" "$DEST/"
cp "$PLUGIN_JSON" "$DEST/plugin.json"
touch "$DEST/.nojekyll"

python3 - "$ROOT" "$DEST" "$PLUGIN_ID" <<'PY'
import json
import shutil
import sys
from pathlib import Path

root = Path(sys.argv[1])
dest = Path(sys.argv[2])
plugin_id = sys.argv[3]
plugin = json.loads((root / "plugin.json").read_text())

suffixes = {
    "nts-1_mkii": ".nts1mkiiunit",
    "nts-3_kaoss": ".nts3unit",
}

builds = []
for target_id in plugin.get("targets", []):
    suffix = suffixes.get(target_id)
    if not suffix:
        continue
    target_dir = root / "targets" / target_id
    unit_name = f"{plugin_id}{suffix}"
    unit_src = target_dir / unit_name
    if not unit_src.is_file():
        raise SystemExit(f"missing unit file: {unit_src}")
    unit_dest = dest / "units" / unit_name
    shutil.copy2(unit_src, unit_dest)

    sim_src = target_dir / "sim"
    wasm_href = None
    if (sim_src / f"{plugin_id}.js").is_file():
        sim_dest = dest / "sim" / target_id
        if sim_dest.exists():
            shutil.rmtree(sim_dest)
        shutil.copytree(sim_src, sim_dest)
        wasm_href = f"./sim/{target_id}/{plugin_id}.html"

    builds.append({
        "target": target_id,
        "unit": f"./units/{unit_name}",
        "wasm": wasm_href,
    })

(dest / "builds.json").write_text(json.dumps({
    "id": plugin_id,
    "builds": builds,
}, indent=2) + "\n")
print(f"assembled {dest}")
for build in builds:
    print(f"  {build['target']}: {build['unit']}" + (f" {build['wasm']}" if build["wasm"] else ""))
PY
