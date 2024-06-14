set -e

for d in examples/*/*.z64 ; do
  n=0
  until [ "$n" -ge 5 ]
  do
     sc64deployer --remote 192.168.0.6:9064 upload --tv=ntsc "$d" && break
     n=$((n+1))
     sleep 2
  done

  curl 192.168.0.6:9065/reset
  read -n 1 -s
done
