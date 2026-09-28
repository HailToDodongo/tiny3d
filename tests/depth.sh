#!/usr/bin/env bash
# Depth precision regression test, requires ares-64 test runner + node.
#
# Usage: tests/depth.sh [--update] [preset ...]
#   --update      accept the current result as the new golden
#   preset        one or more preset numbers (default: 0 2)
#                   0 = near 10 / far 12800 (default-sized)
#                   2 = near 10 / far 150   (near-field)
#
# Runs the SWEEP scene of examples/98_depthtest for each preset (tests/depth.test.js),
# stores the z-buffer code of all 2000 sweep samples in tests/depth/<name>.actual.txt
# and compares it against the golden tests/depth/<name>.txt (tests/depth_compare.js):
# the metrics are always printed side by side, any metric getting worse fails,
# a changed sample vector with equal-or-better metrics is reported as CHANGED.

set -u
cd "$(dirname "$0")/.."

ARES_TEST=${ARES_TEST:-ares-test}
ROM=examples/98_depthtest/t3d_98_depthtest.z64
DIR=tests/depth

declare -A NAMES=([0]=near10_far12800 [1]=near10_far1000 [2]=near10_far150 [3]=near1_far100)

update=""
selected=()
for arg in "$@"; do
  case "$arg" in
    --update) update="--update" ;;
    *) selected+=("$arg") ;;
  esac
done
[[ ${#selected[@]} -eq 0 ]] && selected=(0 2)

if [[ ! -f "$ROM" ]]; then
  echo "FAIL depth (ROM not built: $ROM)"
  exit 1
fi

mkdir -p "$DIR"
rm -f "$DIR"/*.actual.txt
pass=0 fail=0

for p in "${selected[@]}"; do
  name=${NAMES[$p]:-}
  if [[ -z "$name" ]]; then
    echo "FAIL preset $p (unknown)"
    fail=$((fail + 1))
    continue
  fi
  actual="$DIR/$name.actual.txt"
  golden="$DIR/$name.txt"
  echo "== depth preset $p ($name)"
  if ! out=$("$ARES_TEST" tests/depth.test.js "$ROM" "$p" --timeout 900 2>&1); then
    echo "FAIL $name (capture)"
    sed 's/^/  | /' <<<"$out" | tail -20
    fail=$((fail + 1))
    continue
  fi
  grep -q '^WARNING' <<<"$out" && grep '^WARNING' <<<"$out" | sed 's/^/  /'
  sed -n '/^DEPTH-BEGIN$/,/^DEPTH-END$/p' <<<"$out" | sed '1d;$d' >"$actual"
  if node tests/depth_compare.js "$golden" "$actual" $update; then
    echo "PASS $name"
    pass=$((pass + 1))
  else
    echo "FAIL $name"
    fail=$((fail + 1))
  fi
done

echo "----"
echo "pass=$pass fail=$fail"
[[ $fail -eq 0 ]] || exit 1
