#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blep/schemes/rainbow.h"

#include "blep/utils/file.h"
#include "blep/utils/hash.h"
#include "blep/math/matrix.h"

#if RAINBOW_VERSION == 1
    #define GF_MUL gf16_mul

    #define HASH_STRING sha256_string
    #define HASH_FILE sha256_file

    const unsigned short v1 = 36, o1 = 32, o2 = 32, q = 16, element_hex_size = 1;

#elif RAINBOW_VERSION == 2
    #define GF_MUL gf256_mul

    #define HASH_STRING sha384_string
    #define HASH_FILE sha384_file

    const unsigned short v1 = 68, o1 = 32, o2 = 48, q = 256, element_hex_size = 2;

#elif RAINBOW_VERSION == 3
    #define GF_MUL gf256_mul
    
    #define HASH_STRING sha512_string
    #define HASH_FILE sha512_file
    
    const unsigned short v1 = 96, o1 = 36, o2 = 64, q = 256, element_hex_size = 2;

#endif

#define HEX_TO_INT(chr) (chr >= 'a') ? (chr - 'a' + 10) : (chr - '0')

const unsigned short n_variables = v1 + o1 + o2, n_polynomials = n_variables - v1;
const unsigned int N = n_variables * (n_variables + 1) / 2;


void finish_digest(unsigned char message_digest[SHA_DIGEST_SIZE + SALT_SIZE], const unsigned char* salt, unsigned char* output_buffer) {
    // copy the salt at the end of the array
    // digest <- H(msg) || salt
    memcpy(message_digest + SHA_DIGEST_SIZE, salt, SALT_SIZE);

    // temp_buffer <- H( H(msg) || salt )
    HASH_STRING(message_digest, SHA_DIGEST_SIZE + SALT_SIZE, output_buffer);

    // pad the digest with its digest (digestception!)
    // digest = H( H(m) || salt ) || H( H( H(m) || salt ) )
    #if RAINBOW_VERSION != 1
        HASH_STRING(output_buffer, SHA_DIGEST_SIZE, message_digest);
        memcpy(output_buffer + SHA_DIGEST_SIZE, message_digest, n_polynomials - SHA_DIGEST_SIZE);
    #endif
}

void get_message_digest(const unsigned char* message, size_t mlen, const unsigned char* salt, unsigned char *output_buffer) {
    unsigned char temp_buffer[SHA_DIGEST_SIZE + SALT_SIZE];

    // temp_buffer <- H(msg)
    HASH_STRING(message, mlen, temp_buffer);

    finish_digest(temp_buffer, salt, output_buffer);
}

void get_message_digest_from_file(const char* message_path, const unsigned char* salt, unsigned char *output_buffer) {
    unsigned char temp_buffer[SHA_DIGEST_SIZE + SALT_SIZE];

    // temp_buffer <- H(msg)
    HASH_FILE(message_path, temp_buffer);

    finish_digest(temp_buffer, salt, output_buffer);
}

void get_simple_message_digest(const char* message_path, unsigned char *output_buffer) {
    HASH_FILE(message_path, output_buffer);
}
    
void get_complete_digest(const char* message_path, const unsigned char* salt, unsigned char *output_buffer) {
    unsigned char temp_buffer[SHA_DIGEST_SIZE + SALT_SIZE];

    // temp_buffer = H(m)
    get_simple_message_digest(message_path, temp_buffer);

    // copy the salt at the end of the array (digest = H(m) || salt)
    memcpy(temp_buffer + SHA_DIGEST_SIZE, salt, SALT_SIZE);

    // output_buffer = H( H(m) || salt )
    HASH_STRING(temp_buffer, SHA_DIGEST_SIZE + SALT_SIZE, output_buffer);

    // pad the digest with its digest (digestception!)
    #if RAINBOW_VERSION != 1
        // temp_buffer = H( H(m) || salt ) || H( H( H(m) || salt ) )
        HASH_STRING(output_buffer, SHA_DIGEST_SIZE, temp_buffer);
        memcpy(output_buffer + SHA_DIGEST_SIZE, temp_buffer, n_polynomials - SHA_DIGEST_SIZE);
    #endif
}

MatrixDS* get_public_key(const unsigned char *pk) {
    MatrixDS* PK = CreateMatrix(n_polynomials, N, false);

    #if RAINBOW_VERSION == 1
        for (unsigned int j=0; j < N; j++) {
            for (unsigned int i=0; i < n_polynomials; i+=2) {
                set(PK, i, j, (gf) (pk[j*N + i] >> 4));
                set(PK, i, j+1, (gf) (pk[j*N + i] & 0xf));
            }
        }
    #else
        for (unsigned int j=0; j < N; j++)
            for (unsigned int i=0; i < n_polynomials; i++)
                set(PK, i, j, (gf) pk[j*N + i]);
    #endif

    return PK;
}

MatrixDS* get_public_key_from_file(const char *pk_path) {
    MatrixDS* PK = CreateMatrix(n_polynomials, N, false);
    FILE *pk_file = open_file(pk_path, "r");
    
    //consume the header of the pk file
    while (fgetc(pk_file) != '=');
    fgetc(pk_file);

    #if RAINBOW_VERSION == 1
        for (unsigned int j=0; j < N; j++) {
            for (unsigned int i=0; i < n_polynomials; i++) {
                unsigned char temp_char = fgetc(pk_file);
                set(PK, i, j, (gf) (HEX_TO_INT(temp_char))); 
            }
        }
    #else
        for (unsigned int j=0; j < N; j++) {
            for (unsigned int i=0; i < n_polynomials; i++) {
                unsigned char temp_buffer[2];

                if (fread(temp_buffer, sizeof(unsigned char), 2, pk_file) != 2) {
                    fprintf(stderr, "Error during public key file read");
                    exit(EXIT_FAILURE);
                }

                set(PK, i, j, (gf) (((HEX_TO_INT(temp_buffer[0])) << 4) | (HEX_TO_INT(temp_buffer[1])))); 
            }
        }
    #endif

    fclose(pk_file);

    return PK;
}

/*MatrixDS* parse_public_key(const char *pk_path) {
    MatrixDS* PK = CreateMatrix(n_polynomials, N, false);
    FILE *pk_file = open_file(pk_path, "r");

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
}*/

MatrixDS* get_signature(const unsigned char* signature) {
    MatrixDS* s = CreateMatrix(N, 1, false);

    // fill the signature vector
    #if RAINBOW_VERSION == 1
        for (unsigned int i = 0; i < n_variables; i+=2) {
            set(s, i, 0, (gf) (signature[i / 2] >> 4));
            set(s, i+1, 0, (gf) signature[i / 2]);
        }
    #else
        for (unsigned int i = 0; i < n_variables; i++)
            set(s, i, 0, (gf) signature[i]);
    #endif

    // Compute the explicit signature vector with the quadratic products
    unsigned int h = N - 1;

    for (unsigned int i = n_variables; i > 0; i--)
        for (unsigned int j = n_variables; j >= i; j--)
            set(s, h--, 0, GF_MUL(get(s, i-1, 0), get(s, j-1, 0)));

    return s;
}

MatrixDS* get_signature_from_file(const char* signature_path, unsigned char *salt_buffer) {
    MatrixDS* s = CreateMatrix(N, 1, false);

    FILE *signature_file = open_file(signature_path, "r");

    // consume signature header
    while (fgetc(signature_file) != '=');
    fgetc(signature_file);

    unsigned char temp_buffer[2];

    // fill the signature vector
    #if RAINBOW_VERSION == 1
        for (unsigned int i = 0; i < n_variables; i+=2) {
            temp_buffer[0] = fgetc(signature_file);
            temp_buffer[1] = fgetc(signature_file);

            set(s, i, 0, (gf) (HEX_TO_INT(temp_buffer[1])));
            set(s, i+1, 0, (gf) (HEX_TO_INT(temp_buffer[0])));
        }
    #else
        for (unsigned int i = 0; i < n_variables; i++) {
            if (fread(temp_buffer, sizeof(unsigned char), 2, signature_file) != 2) {
                fprintf(stderr, "Error during signature file read");
                exit(EXIT_FAILURE);
            }
            set(s, i, 0, (gf) (((HEX_TO_INT(temp_buffer[0])) << 4) | (HEX_TO_INT(temp_buffer[1]))));
        }
    #endif

    // fill the salt buffer
    for (unsigned int i = 0; i < SALT_SIZE; i++) {
        if (fread(temp_buffer, sizeof(unsigned char), 2, signature_file) != 2) {
            fprintf(stderr, "Error during signature file read");
            exit(EXIT_FAILURE);
        }
        salt_buffer[i] = ((HEX_TO_INT(temp_buffer[0])) << 4) | (HEX_TO_INT(temp_buffer[1]));
    }

    fclose(signature_file);

    // Compute the explicit signature vector with the quadratic products
    unsigned int h = N - 1;

    for (unsigned int i = n_variables; i > 0; i--)
        for (unsigned int j = n_variables; j >= i; j--)
            set(s, h--, 0, GF_MUL(get(s, i-1, 0), get(s, j-1, 0)));

    return s;
}

/*MatrixDS* parse_signature(const char* signature_path, unsigned char *salt_buffer) {
    MatrixDS* s = CreateMatrix(N, 1, false);

    FILE *signature_file = open_file(signature_path, "r");

    // consume signature header
    while (fgetc(signature_file) != '=');
    fgetc(signature_file);

    char buffer[3];

    // fill the signature vector
    #if RAINBOW_VERSION == 1
        for (unsigned int i = 0; i < n_variables; i+=2) {
            fgets(buffer, 3, signature_file);
            set(s, i, 0, (gf) HEX_TO_INT(buffer[1]));
            set(s, i+1, 0, (gf) HEX_TO_INT(buffer[0]));
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
}*/


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
