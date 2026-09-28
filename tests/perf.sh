#!/usr/bin/env bash
# RSP performance tests, requires the ares-64 test runner.
#
# Usage: tests/perf.sh [example ...]
#   example   one or more example names (e.g. 99_testscene); default: every
#             tests/perf/*.json
#
# Each tests/perf/<name>.json holds the frame counts and the baseline times:
#   { "framesWarmup": 40, "framesProfile": 30, "data": {"tiny3d/Tri Strip": 3583.76, ...} }
# The ROM is implied by the file name: examples/<name>/t3d_<name>.z64
#
# Exits non-zero if any command is slower than its baseline. To update a baseline,
# copy the "perfMap:" line printed at the end into the "data" object of that JSON.

set -u
cd "$(dirname "$0")/.."

ARES_TEST=${ARES_TEST:-ares-test}
PERF_DIR=tests/perf

selected=("$@")
if [[ ${#selected[@]} -eq 0 ]]; then
  shopt -s nullglob
  for f in "$PERF_DIR"/*.json; do selected+=("$(basename "$f" .json)"); done
  shopt -u nullglob
  if [[ ${#selected[@]} -eq 0 ]]; then
    echo "no baselines found in $PERF_DIR"
    exit 1
  fi
fi

pass=0 fail=0

for name in "${selected[@]}"; do
  json="$PERF_DIR/$name.json"
  rom="examples/$name/t3d_$name.z64"

  if [[ ! -f "$json" ]]; then
    echo "FAIL $name (no baseline: $json)"
    fail=$((fail + 1))
    continue
  fi
  if [[ ! -f "$rom" ]]; then
    echo "FAIL $name (ROM not built: $rom)"
    fail=$((fail + 1))
    continue
  fi

  # the whole JSON goes to the test script, it reads the frame counts and the baseline
  cfg=$(cat "$json")

  # the example Makefiles do not track the library, a stale ROM would measure old ucode
  if [[ -f build/libt3d.a && build/libt3d.a -nt "$rom" ]]; then
    echo "WARN $name: $rom is older than build/libt3d.a, rebuild it (make -C examples/$name)"
  fi

  echo "=== $name ($rom)"
  if out=$("$ARES_TEST" tests/perf.test.js "$rom" "$cfg" 2>&1); then
    sed 's/^/  /' <<<"$out"
    echo "PASS $name"
    pass=$((pass + 1))
  else
    sed 's/^/  /' <<<"$out"
    echo "FAIL $name"
    fail=$((fail + 1))
  fi
done

echo "----"
echo "pass=$pass fail=$fail"
[[ $fail -eq 0 ]] || exit 1
