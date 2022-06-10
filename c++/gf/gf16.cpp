#include <cstdint>
#include <ostream>

#include "../include/gf16.h"
#include "gf_operations.cpp"
#include "gf16_lookup.cpp"


gf16::gf16(uint8_t v) {
    this -> v = v & 0xf;
}

gf16::gf16() {
    this -> v = 0;
}

gf16::operator bool() const {
    return v;
}

gf16 gf16::operator+(const gf16 &b) const {
    return gf16(gf16_add_lookup[this -> v][b.v]);
}

gf16 gf16::operator+=(const gf16 &b) {
    this -> v = gf16_add_lookup[this -> v][b.v];
    return *this;
}

gf16 gf16::operator-(const gf16 &b) const {
    return gf16(gf16_sub_lookup[this -> v][b.v]);
}

gf16 gf16::operator*(const gf16 &b) const {
    return gf16(gf16_mul_lookup[this -> v][b.v]);
}

/*gf16 operator/(const gf16 &b) const {
    if (b.v == 0) {
        exit(-1);
    }

    return (*this) * gf16(gf16_inv_lookup[b.v - 1]);
}*/

bool gf16::operator==(const gf16 &b) const {
    return v == b.v;
}

bool gf16::operator!=(const gf16 &b) const {
    return v != b.v;
}

std::ostream& operator<<(std::ostream &s, const gf16 &a) {
    return s << (unsigned int) a.v;
}
