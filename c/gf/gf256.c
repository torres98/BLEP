#include "../include/gf256.h"

#if !defined GF256_LOOKUP || GF256_LOOKUP == 0
    #include "../include/gf16.h"
    #include "../include/gf_operations.h"  
#elif GF256_LOOKUP == 1 || GF256_LOOKUP == 2 || GF256_LOOKUP == 3 || GF256_LOOKUP == 4
    #include "gf256_lookup.h"
#else
    #error "Lookup level not recognized (choose between 0, 1, 2, 3 and 4)"
#endif


#if !defined GF256_LOOKUP || GF256_LOOKUP == 0
    gf256 gf256_add(gf256 a, gf256 b) {
        return gf16_add(a >> 4, b >> 4) << 4 | gf16_add(a & 0xf, b & 0xf);
    }

    gf256 gf256_mul(gf256 a, gf256 b) {
        uint8_t a0 = a & 15;
        uint8_t a1 = (a >> 4);
        uint8_t b0 = b & 15;
        uint8_t b1 = (b >> 4);
        uint8_t a0b0 = gf16_mul(a0, b0);
        uint8_t a1b1 = gf16_mul(a1, b1);
        uint8_t a0b1_a1b0 = gf16_mul(a0 ^ a1, b0 ^ b1) ^ a0b0 ^ a1b1;
        uint8_t a1b1_x8 = gf16_mul_8(a1b1);
        return ((a0b1_a1b0 ^ a1b1) << 4) ^ a0b0 ^ a1b1_x8;
    }

#elif GF256_LOOKUP == 1 || GF256_LOOKUP == 2
    #define get_add_index(i, j) ((i*(511 - i)) / 2) + (j - i - 1)
    #define get_mul_index(i, j) 255*(i-1) + ((i*(3 - i)) / 2) + (j - i) - 1

    gf256 gf256_add(gf256 a, gf256 b) {
        if (a < b)
            return gf256_add_lookup[get_add_index(a, b)];
        else if (b < a)
            return gf256_add_lookup[get_add_index(b, a)];
        else
            return 0;
    }

    gf256 gf256_mul(gf256 a, gf256 b)  {
        if (a == 0 || b == 0)
            return 0;
        if (a < b)
            return gf256_mul_lookup[get_mul_index(a, b)];
        else
            return gf256_mul_lookup[get_mul_index(b, a)];
    }

#else
    gf256 gf256_add(gf256 a, gf256 b) {
        return gf256_add_lookup[a][b];
    }

    gf256 gf256_mul(gf256 a, gf256 b) {
        return gf256_mul_lookup[a][b];
    }
    
#endif
