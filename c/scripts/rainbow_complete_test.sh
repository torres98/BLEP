#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "Illegal number of arguments"
    exit -1
fi

for VERSION in {1..3}
do
    if [[ $VERSION -eq 1 ]]; then
        GF="gf16"
        ELEM_BIT_SIZE=4
    else
        GF="gf256"
        ELEM_BIT_SIZE=8
    fi

    for LOOKUP in {0..2}
    do
        echo "Testing Rainbow $VERSION with lookup level $LOOKUP"
        gcc test/rainbow_test.c schemes/rainbow.c gf/gf256.c gf/gf16.c gf/gf_operations.c utils/* -o rainbow_test.out -lcrypto -lm -includeinclude/$GF.h -DRAINBOW_VERSION=$VERSION -DELEM_TYPE=$GF -DELEM_BIT_SIZE=$ELEM_BIT_SIZE -DELEM_ADD=${GF}_add -DELEM_SUB=${GF}_add -DELEM_MUL=${GF}_mul -D${GF^^}_LOOKUP=$LOOKUP -Wall -Wextra

        ./rainbow_test.out $1 $2
        echo
    done
done

rm rainbow_test.out
