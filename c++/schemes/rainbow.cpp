#include <cstdio>

#include "../include/rainbow.h"

#include "../include/hash_utils.h"
#include "../include/math_utils.h"

#define hex_to_int(chr) (chr >= 'a') ? (chr - 'a' + 10) : (chr - '0')

#if RAINBOW_VERSION == 1
    #define HASH_STRING sha256_string
    #define HASH_FILE sha256_file

    #define SHA_DIGEST_SIZE 32
    #define FINAL_DIGEST_SIZE 32

    const unsigned short element_hex_size = 1;

#elif RAINBOW_VERSION == 2
    #define HASH_STRING sha384_string
    #define HASH_FILE sha384_file

    #define SHA_DIGEST_SIZE 48
    #define FINAL_DIGEST_SIZE 80

    const unsigned short element_hex_size = 2;

#elif RAINBOW_VERSION == 3
    #define HASH_STRING sha512_string
    #define HASH_FILE sha512_file

    #define SHA_DIGEST_SIZE 64
    #define FINAL_DIGEST_SIZE 100

    const unsigned short element_hex_size = 2;

#endif

using namespace Rainbow;


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

void Rainbow::get_message_digest(const unsigned char* message, size_t mlen, const unsigned char* salt, unsigned char *output_buffer) {
    unsigned char temp_buffer[SHA_DIGEST_SIZE + SALT_SIZE];

    // temp_buffer <- H(msg)
    HASH_STRING(message, mlen, temp_buffer);

    finish_digest(temp_buffer, salt, output_buffer);
}

void Rainbow::get_message_digest_from_file(const char* message_path, const unsigned char* salt, unsigned char *output_buffer) {
    unsigned char temp_buffer[SHA_DIGEST_SIZE + SALT_SIZE];

    // temp_buffer <- H(msg)
    HASH_FILE(message_path, temp_buffer);

    finish_digest(temp_buffer, salt, output_buffer);
}

MatrixDS<gf> Rainbow::get_public_key(const unsigned char *pk) {
    MatrixDS<gf> PK = MatrixDS<gf>(n_polynomials, N);

    #if RAINBOW_VERSION == 1
        for (unsigned int j=0; j < N; j++) {
            for (unsigned int i=0; i < n_polynomials; i+=2) {
                PK.set(i, j, gf(pk[j*N + i] >> 4));
                PK.set(i, j+1, gf(pk[j*N + i])); // gf16 already performs bit masking
            }
        }
    #else
        for (unsigned int j=0; j < N; j++)
            for (unsigned int i=0; i < n_polynomials; i++)
                PK.set(i, j, gf(pk[j*N + i]));
    #endif

    return PK;
}

MatrixDS<gf> Rainbow::get_public_key_from_file(const char *pk_path) {
    MatrixDS<gf> PK = MatrixDS<gf>(n_polynomials, N);
    FILE *pk_file = fopen(pk_path, "r");

    //consume the header of the pk file
    while (fgetc(pk_file) != '=');
    fgetc(pk_file);

    #if RAINBOW_VERSION == 1
        for (unsigned int j=0; j < N; j++) {
            for (unsigned int i=0; i < n_polynomials; i++) {
                unsigned char temp_char = fgetc(pk_file);
                PK.set(i, j, gf(hex_to_int(temp_char))); 
            }
        }
    #else
        for (unsigned int j=0; j < N; j++) {
            for (unsigned int i=0; i < n_polynomials; i++) {
                unsigned char temp_buffer[2];

                fread(temp_buffer, sizeof(unsigned char), 2, pk_file);
                PK.set(i, j, gf(((hex_to_int(temp_buffer[0])) << 4) | (hex_to_int(temp_buffer[1])))); 
            }
        }
    #endif

    fclose(pk_file);

    return PK;
}


VectorDS<gf> Rainbow::get_signature(const unsigned char* signature) {
    VectorDS<gf> s = VectorDS<gf>(N);

    // fill the signature vector
    #if RAINBOW_VERSION == 1
        for (unsigned int i = 0; i < n_variables; i+=2) {
            s.set(i, gf(signature[i] >> 4));
            s.set(i+1, gf(signature[i]));
        }
    #else
        for (unsigned int i = 0; i < n_variables; i++)
            s.set(i, gf(signature[i]));
    #endif

    // Compute the explicit signature vector with the quadratic products
    unsigned int h = N - 1;

    for (unsigned int i = n_variables; i > 0; i--)
        for (unsigned int j = n_variables; j >= i; j--)
            s.set(h--, s(i-1) * s(j-1));

    return s;
}

VectorDS<gf> Rainbow::get_signature_from_file(const char* signature_path, unsigned char *salt_buffer) {
    VectorDS<gf> s = VectorDS<gf>(N);

    FILE *signature_file = fopen(signature_path, "r");

    // consume signature header
    while (fgetc(signature_file) != '=');
    fgetc(signature_file);

    unsigned char temp_buffer[2];

    // fill the signature vector
    #if RAINBOW_VERSION == 1
        for (unsigned int i = 0; i < n_variables; i+=2) {
            temp_buffer[0] = fgetc(signature_file);
            temp_buffer[1] = fgetc(signature_file);

            s.set(i, gf(hex_to_int(temp_buffer[1])));
            s.set(i+1, gf(hex_to_int(temp_buffer[0])));
        }
    #else
        for (unsigned int i = 0; i < n_variables; i++) {
            fread(temp_buffer, sizeof(unsigned char), 2, signature_file);
            s.set(i, gf(((hex_to_int(temp_buffer[0])) << 4) | (hex_to_int(temp_buffer[1]))));
        }
    #endif

    // fill the salt buffer
    for (unsigned int i = 0; i < SALT_SIZE; i++) {
        fread(temp_buffer, sizeof(unsigned char), 2, signature_file);
        salt_buffer[i] = ((hex_to_int(temp_buffer[0])) << 4) | (hex_to_int(temp_buffer[1]));
    }

    fclose(signature_file);

    // Compute the explicit signature vector with the quadratic products
    unsigned int h = N - 1;

    for (unsigned int i = n_variables; i > 0; i--)
        for (unsigned int j = n_variables; j >= i; j--)
            s.set(h--, s(i-1) * s(j-1));

    return s;
}


VectorDS<gf> build_result_vector(unsigned char *digest) {
    VectorDS<gf> u = VectorDS<gf>(Rainbow::n_polynomials);

    // insert the final digest
    #if RAINBOW_VERSION == 1
        for (unsigned int i = 0; i < Rainbow::n_polynomials; i+=2) {
            u.set(i, gf(digest[i/2] >> 4));
            u.set(i+1, gf(digest[i/2] & 0xf));
        }
    #else
        for (unsigned int i = 0; i < Rainbow::n_polynomials; i++)
            u.set(i, gf(digest[i]));
    #endif

    return u;
}

VectorDS<gf> Rainbow::get_result_vector(const unsigned char* message, size_t mlen, const unsigned char* salt) {
    unsigned char digest[FINAL_DIGEST_SIZE];
    get_message_digest(message, mlen, salt, digest);

    return build_result_vector(digest);
}

VectorDS<gf> Rainbow::get_result_vector_from_file(const char* message_path, const unsigned char* salt) {
    unsigned char digest[FINAL_DIGEST_SIZE];
    get_message_digest_from_file(message_path, salt, digest);

    return build_result_vector(digest);
}
