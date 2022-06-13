#ifndef GF256_H
#define GF256_H

#include <cstdint>
#include <ostream>


class gf256 {
    private:
        uint8_t v;

    public:

        gf256(uint8_t v);

        gf256();

        gf256 operator+(const gf256 &b) const;
        gf256 operator+=(const gf256 &b);
        gf256 operator-(const gf256 &b) const;
        gf256 operator*(const gf256 &b) const;

        operator bool() const;
        
        bool operator==(const gf256 &b) const;

        bool operator!=(const gf256 &b) const;

        friend std::ostream& operator<<(std::ostream &s, const gf256 &a);
};

#endif
