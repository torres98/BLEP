#include "../include/gf_operations.h"


uint8_t gf2_add(uint8_t a, uint8_t b) {
    return a ^ b;
}

uint8_t gf2_mul(uint8_t a, uint8_t b) {
    return a & b;
}


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


uint8_t gf16_mul_8(uint8_t a) {
    uint8_t a0 = a & 3;
    uint8_t a1 = a >> 2;
    return (gf4_mul_2(a0 ^ a1) << 2) | gf4_mul_3(a1);
}
