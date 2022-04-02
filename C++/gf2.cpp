#include <cstdint>

#ifndef _GF2
#define _GF2

uint8_t gf2_add(uint8_t a, uint8_t b) {
    return a ^ b;
}

uint8_t gf2_mul(uint8_t a, uint8_t b) {
    return a & b;
}

/*class gf2 {
    private:
        uint8_t v;

    public:
        
        gf2(uint8_t v) {
            this -> v = v;
        }
        
        operator bool() {
            return v != 0;
        }

        gf2 operator+(gf2 b) {
            return gf2(gf2_add(v, b.v));
        }

        gf2 operator*(gf2 b) {
            return gf2(gf2_mul(v, b.v));
        }
};*/

#endif