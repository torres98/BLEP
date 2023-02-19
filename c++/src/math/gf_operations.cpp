#include <cstdint>

#include "gf_operations.h"


/*******/
/* GF2 */
/*******/

uint8_t gf2_add(uint8_t a, uint8_t b) {
    return a ^ b;
}

uint8_t gf2_mul(uint8_t a, uint8_t b) {
    return a & b;
}


/*******/
/* GF4 */
/*******/

uint8_t gf4_add(uint8_t a, uint8_t b) {
    return gf2_add(a >> 1, b >> 1) << 1 | gf2_add(a & 0x1, b & 0x1);
}

uint8_t gf4_mul_2(uint8_t a) {
    uint8_t r = a << 1;
    r ^= (a >> 1) * 7;
    return r;
}

uint8_t gf4_mul_3(uint8_t a) {
    uint8_t msk = (a - 2) >> 1;
    return (msk & (a * 3)) | ((~msk) & (a - 1));
}

uint8_t gf4_mul(uint8_t a, uint8_t b) {
    uint8_t r = a * (b & 1);
    return r ^ (gf4_mul_2(a) * (b >> 1));
}


/********/
/* GF16 */
/********/

uint8_t gf16_add(uint8_t a, uint8_t b) {
    return gf4_add(a >> 2, b >> 2) << 2 | gf4_add(a & 0x3, b & 0x3);
}

uint8_t gf16_mul_8(uint8_t a) {
    uint8_t a0 = a & 3;
    uint8_t a1 = a >> 2;
    return (gf4_mul_2(a0 ^ a1) << 2) | gf4_mul_3(a1);
}

uint8_t gf16_mul(uint8_t a, uint8_t b) {
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


/*********/
/* GF256 */
/*********/

uint8_t gf256_add(uint8_t a, uint8_t b) {
    return gf16_add(a >> 4, b >> 4) << 4 | gf16_add(a & 0xf, b & 0xf);
}

uint8_t gf256_mul(uint8_t a, uint8_t b) {
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
