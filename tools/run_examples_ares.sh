#!/usr/bin/env bash

set -e

for d in examples/*/*.z64 ; do
  ares "$d"
done
