#include <ostream>

#include "blep/math/gf16.h"

#if !(defined GF16_LOOKUP) || GF16_LOOKUP == 0
    #include "gf_operations.h"
#elif GF16_LOOKUP == 1 || GF16_LOOKUP == 2 || GF16_LOOKUP == 3 || GF16_LOOKUP == 4
    #include "gf16_lookup.h"
#else
    #error "Lookup level not recognized (choose between 0, 1, 2, 3 and 4)"
#endif


#if !(defined GF16_LOOKUP) || GF16_LOOKUP == 0
    gf16 gf16::operator+(const gf16 &b) const {
        return gf16(gf16_add(v_, b.v_));
    }

    gf16 gf16::operator+=(const gf16 &b) {
        v_ = gf16_add(v_, b.v_);
        return *this;
    }

    gf16 gf16::operator-(const gf16 &b) const {
        return gf16(gf16_add(v_, b.v_));
    }

    gf16 gf16::operator*(const gf16 &b) const {
        return gf16(gf16_mul(v_, b.v_));
    }

#elif GF16_LOOKUP == 1 || GF16_LOOKUP == 2
    //      get_add_index(i, j) ((i*(31 - i))  / 2) + (j - i - 1)
    #define get_add_index(i, j) ((i*(31 - i)) >> 1) + (j - i - 1)
    //      get_mul_index(i, j) 15*(i-1) + ((i*(3 - i))  / 2) + (j - i) - 1
    #define get_mul_index(i, j) 15*(i-1) + ((i*(3 - i)) >> 1) + (j - i) - 1

    gf16 gf16::operator+(const gf16 &b) const {
        if (v_ < b.v_) {
            return gf16(gf16_add_lookup[get_add_index(v_, b.v_)]);
        } else if (b.v_ < v_) {
            return gf16(gf16_add_lookup[get_add_index(b.v_, v_)]);
        } else {
            return gf16();
        }
    }

    gf16 gf16::operator+=(const gf16 &b) {
        if (v_ < b.v_) {
            v_ = gf16_add_lookup[get_add_index(v_, b.v_)];
        } else if (b.v_ < v_) {
            v_ = gf16_add_lookup[get_add_index(b.v_, v_)];
        } else {
            v_ = 0;
        }

        return *this;
    }

    gf16 gf16::operator-(const gf16 &b) const {
        return *this + b;
    }

    gf16 gf16::operator*(const gf16 &b) const {
        if (v_ == 0 || b.v_ == 0) {
            return gf16();
        } else if (v_ < b.v_) {
            return gf16(gf16_mul_lookup[get_mul_index(v_, b.v_)]);
        } else {
            return gf16(gf16_mul_lookup[get_mul_index(b.v_, v_)]);
        }
    }

#else
    gf16 gf16::operator+(const gf16 &b) const {
        return gf16(gf16_add_lookup[v_][b.v_]);
    }

    gf16 gf16::operator+=(const gf16 &b) {
        v_ = gf16_add_lookup[v_][b.v_];

        return *this;
    }

    gf16 gf16::operator-(const gf16 &b) const {
        return gf16(gf16_add_lookup[v_][b.v_]);
    }

    gf16 gf16::operator*(const gf16 &b) const {
        return gf16(gf16_mul_lookup[v_][b.v_]);
    }

#endif

gf16::operator bool() const {
    return v_;
}

bool gf16::operator==(const gf16 &b) const {
    return v_ == b.v_;
}

bool gf16::operator!=(const gf16 &b) const {
    return v_ != b.v_;
}

std::ostream& operator<<(std::ostream &s, const gf16 &a) {
    return s << (unsigned short) a.v_;
}
