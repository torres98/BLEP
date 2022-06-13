#include <cstdint>
#include <ostream>

#include "../include/gf256.h"

#ifdef GF256_LOOKUP
    #include "gf256_lookup.h"
#else
    #include "gf_operations.cpp"
#endif


gf256::gf256(uint8_t v) {
    this -> v = v;
}

gf256::gf256() {
    this -> v = 0;
}

#ifdef GF256_LOOKUP
    gf256 gf256::operator+(const gf256 &b) const {
        return gf256(gf256_add_lookup[this -> v][b.v]);
    }

    gf256 gf256::operator+=(const gf256 &b) {
        this -> v = gf256_add_lookup[this -> v][b.v]; //gf256_add(this -> v, b.v);
        return *this;
    }

    gf256 gf256::operator-(const gf256 &b) const {
        return gf256(gf256_add_lookup[this -> v][b.v]);
    }

    gf256 gf256::operator*(const gf256 &b) const {
        return gf256(gf256_mul_lookup[this -> v][b.v]);
    }

    /*gf256 operator/(const gf256 &b) const {
        if (b.v == 0) {
            exit(-1);
        }

        return (*this) * gf256(gf256_inv_lookup[b.v - 1]);
    }*/

#else
    gf256 gf256::operator+(const gf256 &b) const {
        return gf256(gf256_add(this -> v, b.v));
    }

    gf256 gf256::operator+=(const gf256 &b) {
        this -> v = gf256_add(this -> v, b.v);
        return *this;
    }

    gf256 gf256::operator-(const gf256 &b) const {
        return gf256(gf256_add(this -> v, b.v));
    }

    gf256 gf256::operator*(const gf256 &b) const {
        return gf256(gf256_mul(this -> v, b.v));
    }

#endif

gf256::operator bool() const {
    return v != 0;
}

bool gf256::operator==(const gf256 &b) const {
    return v == b.v;
}

bool gf256::operator!=(const gf256 &b) const {
    return v != b.v;
}

std::ostream& operator<<(std::ostream &s, const gf256 &a) {
    return s << (unsigned int) a.v;
}
