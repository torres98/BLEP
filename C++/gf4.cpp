#ifndef _GF4
#define _GF4

#include <cstdint>
#include "gf2.cpp"

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

/*class gf4 {
    public:
        uint8_t _v;

        gf4(uint8_t v) {
            _v = v;
        }
        
        gf4 operator+(gf4 b) {
            return gf4(gf4_add(_v, b._v));
        }

        gf4 operator*(gf4 b) {
            return gf4(gf4_mul(_v, b._v));
        }
};*/

#endif