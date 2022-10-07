#include <ostream>

#include "../include/gf16.h"

#if !(defined GF16_LOOKUP) || GF16_LOOKUP == 0
    #include "gf_operations.cpp"
#elif GF16_LOOKUP == 1 || GF16_LOOKUP == 2 || GF16_LOOKUP == 3 || GF16_LOOKUP == 4
    #include "gf16_lookup.h"
#else
    #error "Lookup level not recognized (choose between 0, 1, 2, 3 and 4)"
#endif


gf16::gf16(uint8_t v) {
    this -> v = v & 0xf;
}


gf16::gf16() {
    this -> v = 0;
}

#if !(defined GF16_LOOKUP) || GF16_LOOKUP == 0
    gf16 gf16::operator+(const gf16 &b) const {
        return gf16(gf16_add(this -> v, b.v));
    }

    gf16 gf16::operator+=(const gf16 &b) {
        this -> v = gf16_add(this -> v, b.v);
        return *this;
    }

    gf16 gf16::operator-(const gf16 &b) const {
        return gf16(gf16_add(this -> v, b.v));
    }

    gf16 gf16::operator*(const gf16 &b) const {
        return gf16(gf16_mul(this -> v, b.v));
    }

#elif GF16_LOOKUP == 1 || GF16_LOOKUP == 2
    #define get_add_index(i, j) ((i*(31 - i)) / 2) + (j - i - 1)
    #define get_mul_index(i, j) 15*(i-1) + ((i*(3 - i)) / 2) + (j - i) - 1

    gf16 gf16::operator+(const gf16 &b) const {
        if (this -> v < b.v)
            return gf16(gf16_add_lookup[get_add_index(this -> v, b.v)]);
        else if (b.v < this -> v)
            return gf16(gf16_add_lookup[get_add_index(b.v, this -> v)]);
        else
            return gf16();
    }

    gf16 gf16::operator+=(const gf16 &b) {
        if (this -> v < b.v)
            this -> v = gf16_add_lookup[get_add_index(this -> v, b.v)];
        else if (b.v < this -> v)
            this -> v = gf16_add_lookup[get_add_index(b.v, this -> v)];
        else
            this -> v = 0;

        return *this;
    }

    gf16 gf16::operator-(const gf16 &b) const {
        return *this + b;
    }

    gf16 gf16::operator*(const gf16 &b) const {
        if (this -> v == 0 || b.v == 0)
            return gf16();
        if (this -> v < b.v)
            return gf16(gf16_mul_lookup[get_mul_index(this -> v, b.v)]);
        else
            return gf16(gf16_mul_lookup[get_mul_index(b.v, this -> v)]);
    }

#else
    gf16 gf16::operator+(const gf16 &b) const {
        return gf16(gf16_add_lookup[this -> v][b.v]);
    }

    gf16 gf16::operator+=(const gf16 &b) {
        this -> v = gf16_add_lookup[this -> v][b.v];

        return *this;
    }

    gf16 gf16::operator-(const gf16 &b) const {
        return gf16(gf16_add_lookup[this -> v][b.v]);
    }

    gf16 gf16::operator*(const gf16 &b) const {
        return gf16(gf16_mul_lookup[this -> v][b.v]);
    }

#endif

gf16::operator bool() const {
    return v;
}

bool gf16::operator==(const gf16 &b) const {
    return v == b.v;
}

bool gf16::operator!=(const gf16 &b) const {
    return v != b.v;
}

std::ostream& operator<<(std::ostream &s, const gf16 &a) {
    return s << (unsigned int) a.v;
}
