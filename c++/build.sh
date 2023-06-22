if [[ $# -ne 2 ]]; then
    echo "Expected 2 args: R (Rainbow version <1, 2, 3>) GFL (gf lookup level <0, 1, 2, 3, 4>)"
    exit 1
fi

if [[ $1 -ne 1 && $1 -ne 2 && $1 -ne 3 ]]; then
    echo "Rainbow version must be one of the following values: 1, 2, 3"
    exit 1
elif [[ $2 -ne 0 && $2 -ne 1 && $2 -ne 2 && $2 -ne 3 && $2 -ne 4 ]]; then
    echo "GF lookup level must be one of the following values: 0, 1, 2, 3, 4"
    exit 1
fi

if [[ $1 -eq 1 ]]; then
    cmake . -DRAINBOW_VERSION:STRING=$1 -DGF16_LOOKUP:STRING=$2 -B build/
else
    cmake . -DRAINBOW_VERSION:STRING=$1 -DGF256_LOOKUP:STRING=$2 -B build/
fi

cmake --build build/ --target all