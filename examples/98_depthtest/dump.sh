#!/usr/bin/env bash
# Run the depth bench once and store its raw measurements as JSON, so that the
# results can be visualised later (tools/depth_report.html) without re-running.
#   examples/98_depthtest/dump.sh <rom.z64> <preset> <out.json> [label]
set -euo pipefail
ROM=$1; PRESET=$2; OUT=$3; LABEL=${4:-$(basename "$ROM" .z64)}
HERE="$(cd "$(dirname "$0")" && pwd)"
TMP="$(mktemp)"
ares-test "$HERE/depthtest.js" "$ROM" "$PRESET" sweep,plane,sep dump "$LABEL" 2>&1 | sed 's/\x1b\[[0-9;]*m//g' > "$TMP" || true
grep -E '^(#### |  |== )' "$TMP" || true
awk '/^DTDUMP-BEGIN$/{f=1; next} /^DTDUMP-END$/{f=0} f' "$TMP" > "$OUT"
rm -f "$TMP"
[ -s "$OUT" ] || { echo "no dump produced" >&2; exit 1; }
echo "wrote $OUT ($(stat -c %s "$OUT") bytes)"
