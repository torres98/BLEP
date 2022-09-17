#include "../include/gf16.h"

#if !defined GF16_LOOKUP || GF16_LOOKUP == 0
    #include "../include/gf_operations.h"
#elif GF16_LOOKUP == 1 || GF16_LOOKUP == 2
    #include "gf16_lookup.h"
#else
    #error "Lookup level not recognized (choose between 0, 1 and 2)"
#endif

#if !defined GF16_LOOKUP || GF16_LOOKUP == 0
    gf16 gf16_add(gf16 a, gf16 b) {
        return gf4_add(a >> 2, b >> 2) << 2 | gf4_add(a & 0x3, b & 0x3);
    }

    gf16 gf16_mul(gf16 a, gf16 b) {
        uint8_t a0 = a & 3;
        uint8_t a1 = (a >> 2);
        uint8_t b0 = b & 3;
        uint8_t b1 = (b >> 2);
        uint8_t a0b0 = gf4_mul(a0, b0);
        uint8_t a1b1 = gf4_mul(a1, b1);
        uint8_t a0b1_a1b0 = gf4_mul(a0 ^ a1, b0 ^ b1) ^ a0b0 ^ a1b1;
        uint8_t a1b1_x2 = gf4_mul_2(a1b1);
        return ((a0b1_a1b0 ^ a1b1) << 2) ^ a0b0 ^ a1b1_x2;
    }
    
#elif GF16_LOOKUP == 1
    #define get_add_index(i, j) ((i*(31 - i)) / 2) + (j - i - 1)
    #define get_mul_index(i, j) 15*(i-1) + ((i*(3 - i)) / 2) + (j - i) - 1

    gf16 gf16_add(gf16 a, gf16 b) {
        if (a < b)
            return gf16_add_lookup[get_add_index(a, b)];
        else if (b < a)
            return gf16_add_lookup[get_add_index(b, a)];
        else
            return 0;
    }

    gf16 gf16_mul(gf16 a, gf16 b)  {
        if (a == 0 || b == 0)
            return 0;
        if (a < b)
            return gf16_mul_lookup[get_mul_index(a, b)];
        else
            return gf16_mul_lookup[get_mul_index(b, a)];
    }

#else
    gf16 gf16_add(gf16 a, gf16 b) {
        return gf16_add_lookup[a][b];
    }

    gf16 gf16_mul(gf16 a, gf16 b) {
        return gf16_mul_lookup[a][b];
    }

#endif
