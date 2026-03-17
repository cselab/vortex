#!/bin/sh

NPROC=$(nproc 2>/dev/null || sysctl -n hw.ncpu)

for d in [0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]/
do if test ! -f $d/status
   then echo $d
   fi
done | xargs -P $NPROC -n 1 sh -xuc '
trap "exit 1" 1 2 15
cd "$0" &&
    date > start &&
    ../vortex $(cat args) \
            2>stderr 1>stdout
echo $? > status
date > end
if test "$(cat status)" = 0
then python3 ../post.py . 2>>stderr
fi
'
