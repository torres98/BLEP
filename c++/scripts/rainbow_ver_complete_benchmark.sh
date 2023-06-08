#!/bin/bash

if [[ $# -ne 3 ]]; then
    echo "Expected 3 args: k (#svk rows) t (#progressive steps) s (#runs)"
    exit 1
fi

if [[ $1 -lt $2 ]]; then
    echo "Number of rows of the SVK cannot be less than the number of progressive steps."
    exit 1
fi

cd ../
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
        cmake . -B=build/ -DRAINBOW_VERSION=$VERSION -D$GF\_LOOKUP=$LOOKUP > /dev/null 2>&1
        cmake --build build/ --target rainbow_ver_benchmark > /dev/null 2>&1

        ./build/scripts/rainbow_ver_benchmark $1 $2 $3
        echo "-------------------------------------------------------------"
    done
done

cmake --build build/ --target clean
