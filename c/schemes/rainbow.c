#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/rainbow.h"

#include "../include/hash_utils.h"
#include "../include/math_utils.h"

#if RAINBOW_VERSION == 1
    #define hex_to_int(chr) (chr >= 'a') ? (chr - 'a' + 10) : (chr - '0')

    #define GF_MUL gf16_mul

    #define HASH_RAW_STR sha256_string
    #define HASH_FILE sha256_file

    const unsigned short v1 = 36, o1 = 32, o2 = 32, q = 16, element_hex_size = 1;

#elif RAINBOW_VERSION == 2
    #define GF_MUL gf256_mul

    #define HASH_RAW_STR sha384_string
    #define HASH_FILE sha384_file

    const unsigned short v1 = 68, o1 = 32, o2 = 48, q = 256, element_hex_size = 2;

#elif RAINBOW_VERSION == 3
    #define GF_MUL gf256_mul
    
    #define HASH_RAW_STR sha512_string
    #define HASH_FILE sha512_file
    
    const unsigned short v1 = 96, o1 = 36, o2 = 64, q = 256, element_hex_size = 2;

#endif

const unsigned short n_variables = v1 + o1 + o2, n_polynomials = n_variables - v1;
const unsigned int N = n_variables * (n_variables + 1) / 2;


void get_message_digest(const char* message_path, unsigned char *output_buffer) {
    HASH_FILE(message_path, output_buffer);
}
    
void get_complete_digest(const char* message_path, const unsigned char* salt, unsigned char *output_buffer) {
    unsigned char temp_buffer[SHA_DIGEST_SIZE + SALT_SIZE];

    // temp_buffer = H(m)
    get_message_digest(message_path, temp_buffer);

    // copy the salt at the end of the array (digest = H(m) || salt)
    memcpy(temp_buffer + SHA_DIGEST_SIZE, salt, SALT_SIZE);

    // output_buffer = H( H(m) || salt )
    HASH_RAW_STR(temp_buffer, output_buffer, SHA_DIGEST_SIZE + SALT_SIZE);

    // pad the digest with its digest (digestception!)
    #if RAINBOW_VERSION != 1
        // temp_buffer = H( H(m) || salt ) || H( H( H(m) || salt ) )
        HASH_RAW_STR(output_buffer, temp_buffer, SHA_DIGEST_SIZE);
        memcpy(output_buffer + SHA_DIGEST_SIZE, temp_buffer, n_polynomials - SHA_DIGEST_SIZE);
    #endif
}

MatrixDS* parse_public_key(const char *pk_path) {
    MatrixDS* PK = CreateMatrix(n_polynomials, N, false);
    FILE *pk_file = fopen(pk_path, "r");

    //consume the header of the pk file
    while (fgetc(pk_file) != '=');
    fgetc(pk_file);

    char element_hex[element_hex_size + 1];

    for (unsigned int j=0; j < N; j++) {
        for (unsigned int i=0; i < n_polynomials; i++) {
            fgets(element_hex, element_hex_size + 1, pk_file);
            set(PK, i, j, (gf) strtol(element_hex, NULL, 16)); 
        }
    }

    fclose(pk_file);

    return PK;
}

MatrixDS* parse_signature(const char* signature_path, unsigned char *salt_buffer) {
    MatrixDS* s = CreateMatrix(N, 1, false);

    FILE *signature_file = fopen(signature_path, "r");

    // consume signature header
    while (fgetc(signature_file) != '=');
    fgetc(signature_file);

    char buffer[3];

    // fill the signature vector
    #if RAINBOW_VERSION == 1
        for (unsigned int i = 0; i < n_variables; i+=2) {
            fgets(buffer, 3, signature_file);
            set(s, i, 0, (gf) hex_to_int(buffer[1]));
            set(s, i+1, 0, (gf) hex_to_int(buffer[0]));
        }

    #else
        for (unsigned int i = 0; i < n_variables; i++) {
            fgets(buffer, 3, signature_file);
            set(s, i, 0, (gf) strtol(buffer, NULL, 16));
        }

    #endif

    // fill the salt buffer
    for (unsigned int i = 0; i < SALT_SIZE; i++) {
        fgets(buffer, 3, signature_file);
        salt_buffer[i] = strtol(buffer, NULL, 16);
    }

    fclose(signature_file);

    // Compute the explicit signature vector with the quadratic products
    unsigned int h = N - 1;

    for (unsigned int i = n_variables; i > 0; i--)
        for (unsigned int j = n_variables; j >= i; j--)
            set(s, h--, 0, GF_MUL(get(s, i-1, 0), get(s, j-1, 0)));

    return s;
}

MatrixDS* get_result_vector(const char* message_path, const unsigned char* salt) {
    MatrixDS* u = CreateMatrix(n_polynomials, 1, false);

    unsigned char digest[FINAL_DIGEST_SIZE]; //fix this

    get_complete_digest(message_path, salt, digest);

    // insert the final digest
    #if RAINBOW_VERSION == 1
        for (unsigned int i = 0; i < n_polynomials; i+=2) {
            set(u, i, 0, (gf) (digest[i/2] >> 4));
            set(u, i+1, 0, (gf) (digest[i/2] & 0xf));
        }

    #else
        for (unsigned int i = 0; i < n_polynomials; i++)
            set(u, i, 0, (gf) digest[i]);
        
    #endif

    return u;
}
