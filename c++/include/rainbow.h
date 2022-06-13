#include <cstdio>

#include "../include/hash_utils.h"
#include "../include/matrix.h"
#include "../include/vector.h"

#ifndef VERSION
    #define VERSION 1
#endif

#if VERSION == 1
    #include "../include/gf16.h"
#elif VERSION == 2 || VERSION == 3
    #include "../include/gf256.h"
#else
    #error "Rainbow version not supported."
#endif

namespace Rainbow {

    #if VERSION == 1
        const unsigned short v1 = 36, o1 = 32, o2 = 32, q = 16, element_hex_size = 1, DIGEST_SIZE = 32;
        typedef gf16 gf;

    #elif VERSION == 2
        const unsigned short v1 = 68, o1 = 32, o2 = 48, q = 256, element_hex_size = 2, DIGEST_SIZE = 48;
        typedef gf256 gf;

    #elif VERSION == 3
        const unsigned short v1 = 96, o1 = 36, o2 = 64, q = 256, element_hex_size = 2, DIGEST_SIZE = 64;
        typedef gf256 gf;

    #endif
    
    const unsigned int n_variables = v1+o1+o2, n_polynomials = n_variables - v1, N = n_variables * (n_variables + 1) / 2, elements_per_byte = 2 / element_hex_size, SALT_SIZE = 16, remaining_bytes = n_polynomials - DIGEST_SIZE * elements_per_byte;

    Matrix<gf> parse_public_key(const char *pk_path);
    Vector<gf> parse_signature(const char* signature_path, unsigned char *salt_buffer);
    Vector<gf> get_result_vector(const char* message_path, const unsigned char* salt);

    void get_message_digest(const char* message_path, unsigned char *output_buffer);
    void get_complete_digest(const char* message_path, const unsigned char* salt, unsigned char *output_buffer);

};
