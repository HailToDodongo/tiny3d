#!/usr/bin/env bash
# Build a self-contained depth report (tools/depth_report.html with all dumps embedded)
# and print its path. Open it directly in a browser, no server needed.
#   examples/98_depthtest/report.sh [out.html] [dump.json ...]   (default: all of data/*_p0.json)
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
OUT="${1:-$HERE/report.html}"; shift || true
if [ $# -eq 0 ]; then set -- "$HERE"/data/tiny3d_noNR_p0.json "$HERE"/data/tiny3d_nrfix_p0.json "$HERE"/data/f3dex3_p0.json "$HERE"/data/f3dex3_zfrac{6,4,3,2,1,0}_p0.json; fi
python3 "$HERE/../../tools/depth_report_embed.py" "$OUT" "$@"
