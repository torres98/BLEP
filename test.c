#include <stdint.h>
#include <stdio.h>

uint64_t carryless_product(uint64_t a, uint64_t b, uint16_t bit_size) {
    uint64_t result = 0;

    for (int i = 0; i < bit_size; i++)
        if ((a >> i) & 1)
            result ^= (b << i);

    return result;
}

void main() {
    printf("%lu\n", carryless_product(2, 7, 4) % 19);
}