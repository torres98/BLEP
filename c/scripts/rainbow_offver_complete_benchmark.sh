#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "Expected 2 args: k (#svk rows) s (#runs)"
    exit 1
fi

set -e

for VERSION in {1..3}
do
    if [[ $VERSION -eq 1 ]]; then
        GF="GF16"
    else
        GF="GF256"
    fi

    for LOOKUP in 0 2 4
    do
        cmake . -B=build_script/ -DRAINBOW_VERSION=$VERSION -D$GF\_LOOKUP=$LOOKUP > /dev/null 2>&1
        cmake --build build_script/ --target rainbow_offver_benchmark > /dev/null 2>&1

        ./build_script/scripts/rainbow_offver_benchmark $1 $2 $3
        echo "-------------------------------------------------------------"
    done
done

cmake --build build_script/ --target clean
rm -rf build_script
