#!/bin/sh

: ${QCC=qcc}
if ! command >/dev/null -v "$QCC"
then printf 'ellipses.sh: error: '\''%s'\'' is not in PATH\n' "$QCC"
     exit 1
fi

a=0.200
b=0.025
c=0.010
"$QCC" -disable-dimensions ellipses.c -O2 -lm -o ellipses &&
./ellipses \
   0.5 0.7 1 $a $b 0 \
   0.5 0.3 1 $a $b 0 \
   0.7 0.5 1 $b $a 0 \
   0.3 0.5 1 $b $a 0 \
   0.7 0.7 1 $c $c 0 \
   0.3 0.7 1 $c $c 0 \
   0.7 0.3 1 $c $c 0 \
   0.3 0.3 1 $c $c 0 &&
python3 post0.py
