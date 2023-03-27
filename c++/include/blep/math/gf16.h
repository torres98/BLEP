#ifndef GF16_H
#define GF16_H

#include <cstdint>
#include <ostream>


class gf16 {
    private:
        uint8_t v_;

    public:
        gf16(uint8_t v=0);

        gf16 operator+(const gf16 &b) const;
        gf16 operator+=(const gf16 &b);
        gf16 operator-(const gf16 &b) const;
        gf16 operator*(const gf16 &b) const;

        operator bool() const;
        
        bool operator==(const gf16 &b) const;
        bool operator!=(const gf16 &b) const;

        friend std::ostream& operator<<(std::ostream &s, const gf16 &a);
};

#endif
