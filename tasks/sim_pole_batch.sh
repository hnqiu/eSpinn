#!/bin/bash

# run pole balancing problem in batch mode
for i in {1..20}
do
    echo "run #$i"
    # run sim
    ./build/eSpinn.sim $1
    # exit if no argument
    return_code=$?
    if [ $return_code -gt 0 ]; then
        echo "exit"
        exit $return_code
    fi
    # archive generated files
    echo "archiving files ..."
    curtime=$(date +"%m%d%H%M%S")
    mkdir asset/cartpole_"$curtime"
    mv asset/archive/* asset/cartpole_"$curtime"/
    mv asset/data/fit asset/cartpole_"$curtime"/
    cp asset/data/force asset/data/states asset/cartpole_"$curtime"/

    # sleep for 1s
    sleep 1
done
