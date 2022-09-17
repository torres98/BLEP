#include <cstdio>

#include "../include/rainbow.h"

#include "../include/hash_utils.h"
#include "../include/math_utils.h"

#if RAINBOW_VERSION == 1
    #define hex_to_int(chr) (chr >= 'a') ? (chr - 'a' + 10) : (chr - '0')

    #define HASH_RAW_STR sha256_string
    #define HASH_FILE sha256_file

    #define SHA_DIGEST_SIZE 32
    #define FINAL_DIGEST_SIZE 32

    const unsigned short element_hex_size = 1;

#elif RAINBOW_VERSION == 2
    #define HASH_RAW_STR sha384_string
    #define HASH_FILE sha384_file

    #define SHA_DIGEST_SIZE 48
    #define FINAL_DIGEST_SIZE 80

    const unsigned short element_hex_size = 2;

#elif RAINBOW_VERSION == 3
    #define HASH_RAW_STR sha512_string
    #define HASH_FILE sha512_file

    #define SHA_DIGEST_SIZE 64
    #define FINAL_DIGEST_SIZE 100

    const unsigned short element_hex_size = 2;

#endif

using Rainbow::gf;


void Rainbow::get_message_digest(const char* message_path, unsigned char *output_buffer) {
    HASH_FILE(message_path, output_buffer);
}
    
void Rainbow::get_complete_digest(const char* message_path, const unsigned char* salt, unsigned char *output_buffer) {
    unsigned char temp_buffer[SHA_DIGEST_SIZE + SALT_SIZE];

    // temp_buffer = H(m)
    get_message_digest(message_path, temp_buffer);

    // copy the salt at the end of the array (digest = H(m) || salt)
    memcpy(temp_buffer + SHA_DIGEST_SIZE, salt, SALT_SIZE);

    // temp_buffer = H( H(m) || salt )
    HASH_RAW_STR(temp_buffer, output_buffer, SHA_DIGEST_SIZE + SALT_SIZE);

    // pad the digest with its digest (digestception!)
    // digest = H( H(m) || salt ) || H( H( H(m) || salt ) )
    #if RAINBOW_VERSION != 1
        HASH_RAW_STR(output_buffer, temp_buffer, SHA_DIGEST_SIZE);
        memcpy(output_buffer + SHA_DIGEST_SIZE, temp_buffer, n_polynomials - SHA_DIGEST_SIZE);
    #endif
}


MatrixDS<gf> Rainbow::parse_public_key(const char *pk_path) {
    MatrixDS<gf> PK = MatrixDS<gf>(n_polynomials, N, true);
    FILE *pk_file = fopen(pk_path, "r");

    //consume the header of the pk file
    while (fgetc(pk_file) != '=');
    fgetc(pk_file);

    char element_hex[element_hex_size + 1];

    for (unsigned int j=0; j < N; j++) {
        for (unsigned int i=0; i < n_polynomials; i++) {
            fgets(element_hex, element_hex_size + 1, pk_file);
            PK.set(i, j, gf(strtol(element_hex, NULL, 16))); 
        }
    }

    fclose(pk_file);

    return PK;
}

VectorDS<gf> Rainbow::parse_signature(const char* signature_path, unsigned char *salt_buffer) {

    VectorDS<gf> s = VectorDS<gf>(N);

    FILE *signature_file = fopen(signature_path, "r");

    // consume signature header
    while (fgetc(signature_file) != '=');
    fgetc(signature_file);

    char buffer[3];

    // fill the signature vector
    #if RAINBOW_VERSION == 1
        for (unsigned int i = 0; i < n_variables; i+=2) {
            fgets(buffer, 3, signature_file);
            s.set(i, gf(hex_to_int(buffer[1])));
            s.set(i+1, gf(hex_to_int(buffer[0])));
        }
    #else
        for (unsigned int i = 0; i < n_variables; i++) {
            fgets(buffer, 3, signature_file);
            s.set(i, gf(strtol(buffer, NULL, 16)));
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
            s.set(h--, s(i-1) * s(j-1));

    return s;

}

VectorDS<gf> Rainbow::get_result_vector(const char* message_path, const unsigned char* salt) {
    VectorDS<gf> u = VectorDS<gf>(n_polynomials);

    unsigned char digest[FINAL_DIGEST_SIZE]; //fix this

    get_complete_digest(message_path, salt, digest);

    // insert the final digest
    #if RAINBOW_VERSION == 1
        for (unsigned int i = 0; i < n_polynomials; i+=2) {
            u.set(i, gf(digest[i/2] >> 4));
            u.set(i+1, gf(digest[i/2] & 0xf));
        }
    #else
        for (unsigned int i = 0; i < n_polynomials; i++)
            u.set(i, gf(digest[i]));
    #endif

    return u;
}
