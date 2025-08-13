#!/usr/bin/env bash
set -e

temp_dir=$(mktemp -d)
find examples -name '*.z64' -exec cp {} "$temp_dir" \;

zip -j -r examples.zip "$temp_dir"/*
rm -rf "$temp_dir"

