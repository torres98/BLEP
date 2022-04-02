#include <cstdint>
#include <ostream>

#include "gf16.cpp"
#include "gf256_lookup.cpp"

#ifndef _GF256
#define _GF256

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

class gf256 {
    private:
        uint8_t v;

    public:

        gf256(uint8_t v) {
            this -> v = v;
        }

        gf256() {
            this -> v = 0;
        }

        operator bool() {
            return v;
        }

        gf256 operator+(gf256 b) {
            return gf256(gf256_add_lookup[this -> v][b.v]);
        }

        gf256 operator+=(gf256 b) {
            this -> v = gf256_add_lookup[this -> v][b.v]; //gf256_add(this -> v, b.v);
            return *this;
        }

        gf256 operator*(gf256 b) {
            return gf256(gf256_mul_lookup[this -> v][b.v]);
        }

        friend std::ostream& operator<<(std::ostream &s, const gf256 &a);
};


std::ostream& operator<<(std::ostream &s, const gf256 &a) {
    return s << (unsigned int) a.v;
}

#endif