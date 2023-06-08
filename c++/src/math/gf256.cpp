#include <ostream>

#include "blep/math/gf256.h"

#if !(defined GF256_LOOKUP) || GF256_LOOKUP == 0
    #include "gf_operations.h"
#elif GF256_LOOKUP == 1 || GF256_LOOKUP == 2 || GF256_LOOKUP == 3 || GF256_LOOKUP == 4
    #include "gf256_lookup.h"
#else
    #error "Lookup level not recognized (choose between 0, 1, 2, 3 and 4)"
#endif


#if !(defined GF256_LOOKUP) || GF256_LOOKUP == 0
    gf256 gf256::operator+(const gf256 &b) const {
        return gf256(gf256_add(v_, b.v_));
    }

    gf256 gf256::operator+=(const gf256 &b) {
        v_ = gf256_add(v_, b.v_);
        return *this;
    }

    gf256 gf256::operator-(const gf256 &b) const {
        return gf256(gf256_add(v_, b.v_));
    }

    gf256 gf256::operator*(const gf256 &b) const {
        return gf256(gf256_mul(v_, b.v_));
    }

#elif GF256_LOOKUP == 1 || GF256_LOOKUP == 2
    //#define get_add_index(i, j) ((i*(511 - i)) / 2) + (j - i - 1)
    #define get_add_index(i, j) ((i*(0x1ff ^ i)) >> 1) + (j - i - 1)
    //#define get_mul_index(i, j) 255*(i-1) + ((i*(3 - i)) / 2) + (j - i) - 1
    #define get_mul_index(i, j) 255*(i-1) + ((i*(3 - i)) >> 1) + (j - i) - 1

    gf256 gf256::operator+(const gf256 &b) const {
        if (v_ < b.v_) {
            return gf256(gf256_add_lookup[get_add_index(v_, b.v_)]);
        } else if (b.v_ < v_) {
            return gf256(gf256_add_lookup[get_add_index(b.v_, v_)]);
        } else {
            return gf256();
        }
    }

    gf256 gf256::operator+=(const gf256 &b) {
        if (v_ < b.v_) {
            v_ = gf256_add_lookup[get_add_index(v_, b.v_)];
        } else if (b.v_ < v_) {
            v_ = gf256_add_lookup[get_add_index(b.v_, v_)];
        } else {
            v_ = 0;
        }

        return *this;
    }

    gf256 gf256::operator-(const gf256 &b) const {
        return *this + b;
    }

    gf256 gf256::operator*(const gf256 &b) const {
        if (v_ == 0 || b.v_ == 0) {
            return gf256();
        } else if (v_ < b.v_) {
            return gf256(gf256_mul_lookup[get_mul_index(v_, b.v_)]);
        } else {
            return gf256(gf256_mul_lookup[get_mul_index(b.v_, v_)]);
        }
    }

#else
    gf256 gf256::operator+(const gf256 &b) const {
        return gf256(gf256_add_lookup[v_][b.v_]);
    }

    gf256 gf256::operator+=(const gf256 &b) {
        v_ = gf256_add_lookup[v_][b.v_];

        return *this;
    }

    gf256 gf256::operator-(const gf256 &b) const {
        return gf256(gf256_add_lookup[v_][b.v_]);
    }

    gf256 gf256::operator*(const gf256 &b) const {
        return gf256(gf256_mul_lookup[v_][b.v_]);
    }

#endif

gf256::operator bool() const {
    return v_ != 0;
}

bool gf256::operator==(const gf256 &b) const {
    return v_ == b.v_;
}

bool gf256::operator!=(const gf256 &b) const {
    return v_ != b.v_;
}

std::ostream& operator<<(std::ostream &s, const gf256 &a) {
    return s << (unsigned short) a.v_;
}
