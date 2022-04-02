#include <cstdint>
#include <ostream>

#include "gf4.cpp"
#include "gf16_lookup.cpp"

#ifndef _GF16
#define _GF16

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

class gf16 {
    private:
        uint8_t v;

    public:

        gf16(uint8_t v) {
            this -> v = v & 0xf;
        }

        gf16() {
            this -> v = 0;
        }

        operator bool() {
            return v;
        }

        gf16 operator+(gf16 b) {
            return gf16(gf16_add_lookup[this -> v][b.v]);
        }

        gf16 operator+=(gf16 b) {
            this -> v = gf16_add_lookup[this -> v][b.v];
            return *this;
        }

        gf16 operator*(gf16 b) {
            return gf16(gf16_mul_lookup[this -> v][b.v]);
        }

        bool operator==(const gf16 &b) {
            return v == b.v;
        }

        friend std::ostream& operator<<(std::ostream &s, const gf16 &a);

};

std::ostream& operator<<(std::ostream &s, const gf16 &a) {
    return s << (a.v & 0xf);
}

#endif