#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "Expected 2 argument, got $#."
    exit -1
fi

if [[ $1 -lt $2 ]]; then
    echo "Number of rows of the SVK cannot be less than the number of progressive steps."
    exit -1
fi

for VERSION in {1..3}
do
    if [[ $VERSION -eq 1 ]]; then
        GF="GF16"
    else
        GF="GF256"
    fi

    for LOOKUP in {0..2}
    do
        echo "Testing Rainbow $VERSION with lookup level $LOOKUP"

        cmake . -B=build/ -DRAINBOW_VERSION=$VERSION -D$GF\_LOOKUP=$LOOKUP > /dev/null 2>&1
        cmake --build build/ --target rainbow_test  > /dev/null 2>&1

        ./build/tests/rainbow_test $1 $2
        echo
    done
done

cmake --build build/ --target clean
