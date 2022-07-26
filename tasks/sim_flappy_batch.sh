#!/bin/bash
for i in {1..10}
do
    echo "Run #$i"
    python tasks/sim_flappy.py -t
done
