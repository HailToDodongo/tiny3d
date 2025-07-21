#!/usr/bin/env bash
set -e

temp_dir=$(mktemp -d)
find examples -name '*.z64' -exec cp {} "$temp_dir" \;
zip -r example_roms.zip "$temp_dir"/*
rm -rf "$temp_dir"

