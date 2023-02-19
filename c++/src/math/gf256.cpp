#include <ostream>

#include "blep/math/gf256.h"

#if !(defined GF16_LOOKUP) || GF16_LOOKUP == 0
    #include "gf_operations.h"
#elif GF256_LOOKUP == 1 || GF256_LOOKUP == 2 || GF256_LOOKUP == 3 || GF256_LOOKUP == 3
    #include "gf256_lookup.h"
#else
    #error "Lookup level not recognized (choose between 0, 1, 2, 3 and 4)"
#endif


gf256::gf256(uint8_t v) {
    this -> v = v;
}

gf256::gf256() {
    this -> v = 0;
}

#if !(defined GF256_LOOKUP) || GF256_LOOKUP == 0
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

#elif GF256_LOOKUP == 1 || GF256_LOOKUP == 2
    #define get_add_index(i, j) ((i*(511 - i)) / 2) + (j - i - 1)
    #define get_mul_index(i, j) 255*(i-1) + ((i*(3 - i)) / 2) + (j - i) - 1

    gf256 gf256::operator+(const gf256 &b) const {
        if (this -> v < b.v)
            return gf256(gf256_add_lookup[get_add_index(this -> v, b.v)]);
        else if (b.v < this -> v)
            return gf256(gf256_add_lookup[get_add_index(b.v, this -> v)]);
        else
            return gf256();
    }

    gf256 gf256::operator+=(const gf256 &b) {
        if (this -> v < b.v)
            this -> v = gf256_add_lookup[get_add_index(this -> v, b.v)];
        else if (b.v < this -> v)
            this -> v = gf256_add_lookup[get_add_index(b.v, this -> v)];
        else
            this -> v = 0;

        return *this;
    }

    gf256 gf256::operator-(const gf256 &b) const {
        return *this + b;
    }

    gf256 gf256::operator*(const gf256 &b) const {
        if (this -> v == 0 || b.v == 0)
            return gf256();
        if (this -> v < b.v)
            return gf256(gf256_mul_lookup[get_mul_index(this -> v, b.v)]);
        else
            return gf256(gf256_mul_lookup[get_mul_index(b.v, this -> v)]);
    }

#else
    gf256 gf256::operator+(const gf256 &b) const {
        return gf256(gf256_add_lookup[this -> v][b.v]);
    }

    gf256 gf256::operator+=(const gf256 &b) {
        this -> v = gf256_add_lookup[this -> v][b.v];

        return *this;
    }

    gf256 gf256::operator-(const gf256 &b) const {
        return gf256(gf256_add_lookup[this -> v][b.v]);
    }

    gf256 gf256::operator*(const gf256 &b) const {
        return gf256(gf256_mul_lookup[this -> v][b.v]);
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
