#include <cstdint>

// GF2
uint8_t gf2_add(uint8_t a, uint8_t b);
uint8_t gf2_mul(uint8_t a, uint8_t b);

// GF4
uint8_t gf4_add(uint8_t a, uint8_t b);
uint8_t gf4_mul_2(uint8_t a);
uint8_t gf4_mul_3(uint8_t a);
uint8_t gf4_mul(uint8_t a, uint8_t b);

// GF16
uint8_t gf16_mul_8(uint8_t a);
