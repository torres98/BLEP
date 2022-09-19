#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "Illegal number of arguments"
    exit -1
fi

for VERSION in {1..3}
do
    if [[ $VERSION -eq 1 ]]; then
        GF="gf16"
    else
        GF="gf256"
    fi

    for LOOKUP in {0..2}
    do
        echo "Testing Rainbow $VERSION with lookup level $LOOKUP"
        g++ tests/rainbow_test.cpp gf/$GF.cpp utils/* schemes/rainbow.cpp -o rainbow_test.out -lcrypto -DRAINBOW_VERSION=$VERSION -D${GF^^}_LOOKUP=$LOOKUP -Wall -Wextra
        
        ./rainbow_test.out $1 $2
        echo
    done
done

rm rainbow_test.out
