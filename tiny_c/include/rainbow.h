#ifndef RAINBOW_H
#define RAINBOW_H

#include <zephyr/device.h>

#include "../include/math_utils.h"

#ifndef RAINBOW_VERSION
    #define RAINBOW_VERSION 1
#endif

#if RAINBOW_VERSION == 1
    #include "../include/gf16.h"
    typedef gf16 gf;

    #define SHA_DIGEST_SIZE 32
    #define FINAL_DIGEST_SIZE 32

#elif RAINBOW_VERSION == 2
    #include "../include/gf256.h"
    typedef gf256 gf;

    #define SHA_DIGEST_SIZE 48
    #define FINAL_DIGEST_SIZE 80

#elif RAINBOW_VERSION == 3
    #include "../include/gf256.h"
    typedef gf256 gf;

    #define SHA_DIGEST_SIZE 64
    #define FINAL_DIGEST_SIZE 100
    
#else
    #error "Rainbow version not supported."
#endif

#define SALT_SIZE 16

extern const unsigned short v1, o1, o2, q, n_variables, n_polynomials;
extern const unsigned int N;

void get_message_digest(const unsigned char* message, unsigned char *output_buffer, unsigned int mlen);
//void get_message_digest(unsigned char *output_buffer, unsigned int mlen);
void get_complete_digest(unsigned char *output_buffer, const unsigned char *message_digest, const unsigned char* salt);
void get_complete_digest_from_device(unsigned char *output_buffer, const unsigned char* salt, unsigned int mlen);

MatrixDS* parse_signature(unsigned char *salt_buffer);
MatrixDS* get_result_vector(const unsigned char* digest);

#endif

