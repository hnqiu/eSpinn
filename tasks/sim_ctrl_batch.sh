#!/bin/bash
for i in {1..10}
do
    echo "Run #$i"
    ./build/eSpinn.sim
    ./asset/bkp_arch.sh
done
