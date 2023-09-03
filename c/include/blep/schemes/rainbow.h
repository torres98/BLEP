#ifndef RAINBOW_H
#define RAINBOW_H

#include <stddef.h>

#ifndef RAINBOW_VERSION
    #define RAINBOW_VERSION 1
#endif

#if RAINBOW_VERSION == 1
    #include "blep/math/gf16.h"
    typedef gf16 gf;
    
    #define SHA_DIGEST_SIZE 32
    #define FINAL_DIGEST_SIZE 32

#elif RAINBOW_VERSION == 2
    #include "blep/math/gf256.h"
    typedef gf256 gf;

    #define SHA_DIGEST_SIZE 48
    #define FINAL_DIGEST_SIZE 80

#elif RAINBOW_VERSION == 3
    #include "blep/math/gf256.h"
    typedef gf256 gf;

    #define SHA_DIGEST_SIZE 64
    #define FINAL_DIGEST_SIZE 100
    
#else
    #error "Rainbow version not supported."
#endif

#define SALT_SIZE 16 //maybe move to .c file

#define ELEM_TYPE gf
#include "blep/math/matrix.h"

extern const unsigned short v1, o1, o2, q, n_variables, n_polynomials;
extern const unsigned int N;

void get_message_digest(const unsigned char* message, size_t mlen, const unsigned char* salt, unsigned char *output_buffer);
void get_message_digest_from_file(const char* message_path, const unsigned char* salt, unsigned char *output_buffer);

//void get_message_digest(const char* message_path, unsigned char *output_buffer);
//void get_complete_digest(const char* message_path, const unsigned char* salt, unsigned char *output_buffer);

MatrixDS* get_public_key(const unsigned char *pk);
MatrixDS* get_public_key_from_file(const char *pk_path);

//MatrixDS* parse_public_key(const char *pk_path);

MatrixDS* get_signature(const unsigned char* signature);
MatrixDS* get_signature_from_file(const char* signature_path, unsigned char *salt_buffer);

//MatrixDS* parse_signature(const char* signature_path, unsigned char *salt_buffer);
MatrixDS* get_result_vector(const char* message_path, const unsigned char* salt);

#endif
