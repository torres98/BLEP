#ifndef GF16_H
#define GF16_H

#include <stdint.h>


typedef uint8_t gf16;

gf16 gf16_add(gf16 a, gf16 b);
gf16 gf16_mul(gf16 a, gf16 b);

#endif
