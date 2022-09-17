#ifndef GF256_H
#define GF256_H

#include <stdint.h>


typedef uint8_t gf256;

gf256 gf256_add(gf256 a, gf256 b);
gf256 gf256_mul(gf256 a, gf256 b);

#endif
