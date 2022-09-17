#include <device.h>
#include <drivers/uart.h> 

#include "../include/uart_utils.h"
#include "../include/hash_utils.h"
#include "../include/math_utils.h"

#ifndef RAINBOW_VERSION
    #define RAINBOW_VERSION 3
#endif

#if RAINBOW_VERSION == 1
    #include "../include/gf16.h"
#elif RAINBOW_VERSION == 2 || RAINBOW_VERSION == 3
    #include "../include/gf256.h"
#else
    #error "Rainbow version not supported."
#endif


namespace Rainbow {
    #if RAINBOW_VERSION == 1
        const unsigned short v1 = 36, o1 = 32, o2 = 32, q = 16, SHA_DIGEST_SIZE = 32, FINAL_DIGEST_SIZE = 32;
        typedef gf16 gf;

    #elif RAINBOW_VERSION == 2
        const unsigned short v1 = 68, o1 = 32, o2 = 48, q = 256, SHA_DIGEST_SIZE = 48, FINAL_DIGEST_SIZE = 80;
        typedef gf256 gf;

    #else
        const unsigned short v1 = 96, o1 = 36, o2 = 64, q = 256, SHA_DIGEST_SIZE = 64, FINAL_DIGEST_SIZE = 100;
        typedef gf256 gf;

    #endif
    
    const unsigned short n_variables = v1 + o1 + o2, n_polynomials = n_variables - v1, SALT_SIZE = 16;
    const unsigned int N = n_variables * (n_variables + 1) / 2;


    void get_message_digest(const struct device *uart_dev, unsigned char *output_buffer, unsigned int mlen);
    void get_complete_digest(unsigned char *output_buffer, const unsigned char *message_digest, const unsigned char* salt);
    void get_complete_digest(const struct device *uart_dev, unsigned char *output_buffer, const unsigned char* salt, unsigned int mlen);

    VectorDS<gf> parse_signature(const struct device *uart_dev, unsigned char *salt_buffer);
    VectorDS<gf> get_result_vector(const unsigned char* digest);
};
