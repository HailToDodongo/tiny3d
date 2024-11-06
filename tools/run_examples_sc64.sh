#!/usr/bin/env bash

set -e
for d in examples/*/*.z64 ; do
  curl 192.168.0.6:9065/off
  sleep 1

  n=0
  until [ "$n" -ge 5 ]
  do
     sc64deployer --remote 192.168.0.6:9064 upload --tv=ntsc "$d" && break
     n=$((n+1))
     sleep 2
  done

  curl 192.168.0.6:9065/on
  read -n 1 -s
done
