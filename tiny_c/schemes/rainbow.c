#include <string.h> //for memcpy

#include <zephyr/device.h>
#include <zephyr/drivers/uart.h> 

#include "../include/rainbow.h"
#include "../include/hash_utils.h"
#include "../include/math_utils.h"
#include "../include/random_utils.h"
#include "../include/uart_utils.h"

#if RAINBOW_VERSION == 1
    #define GF_MUL gf16_mul

    #define HASH_RAW_STR sha256_raw_str
    #define HASH_FROM_DEVICE sha256_from_device

    const unsigned short v1 = 36, o1 = 32, o2 = 32, q = 16, element_hex_size = 1;

#elif RAINBOW_VERSION == 2
    #define GF_MUL gf256_mul

    #define HASH_RAW_STR sha384_raw_str
    #define HASH_FROM_DEVICE sha384_from_device

    const unsigned short v1 = 68, o1 = 32, o2 = 48, q = 256, element_hex_size = 2;

#elif RAINBOW_VERSION == 3
    #define GF_MUL gf256_mul

    #define HASH_RAW_STR sha512_raw_str
    #define HASH_FROM_DEVICE sha512_from_device

    const unsigned short v1 = 96, o1 = 36, o2 = 64, q = 256, element_hex_size = 2;

#endif

const unsigned short n_variables = v1+o1+o2, n_polynomials = n_variables - v1;
const unsigned int N = n_variables * (n_variables + 1) / 2;

void get_message_digest(const unsigned char* message, unsigned char *output_buffer, unsigned int mlen) {
    HASH_RAW_STR(message, output_buffer, mlen);
}

void get_message_digest_from_device(unsigned char *output_buffer, unsigned int mlen) {
    send_ok();
    
    HASH_FROM_DEVICE(output_buffer, mlen);
}

void get_complete_digest(unsigned char *output_buffer, const unsigned char *message_digest, const unsigned char* salt) {
    unsigned char temp_buffer[SHA_DIGEST_SIZE + SALT_SIZE];

    // temp_buffer = H(m) || salt
    memcpy(temp_buffer, message_digest, SHA_DIGEST_SIZE);
    memcpy(temp_buffer + SHA_DIGEST_SIZE, salt, SALT_SIZE);
    
    HASH_RAW_STR(temp_buffer, output_buffer, SHA_DIGEST_SIZE + SALT_SIZE);

    #if RAINBOW_VERSION != 1
        HASH_RAW_STR(output_buffer, temp_buffer, SHA_DIGEST_SIZE);
        memcpy(output_buffer + SHA_DIGEST_SIZE, temp_buffer, n_polynomials - SHA_DIGEST_SIZE);
    #endif
}

void get_complete_digest_from_device(unsigned char *output_buffer, const unsigned char* salt, unsigned int mlen) {
    unsigned char temp_buffer[SHA_DIGEST_SIZE + SALT_SIZE];

    get_message_digest_from_device(temp_buffer, mlen);
    
    // copy the salt at the end of the array (digest = H(m) || salt)
    memcpy(temp_buffer + SHA_DIGEST_SIZE, salt, SALT_SIZE);

    HASH_RAW_STR(temp_buffer, output_buffer, SHA_DIGEST_SIZE + SALT_SIZE);

    #if RAINBOW_VERSION != 1
        HASH_RAW_STR(output_buffer, temp_buffer, SHA_DIGEST_SIZE);
        memcpy(output_buffer + SHA_DIGEST_SIZE, temp_buffer, n_polynomials - SHA_DIGEST_SIZE);
    #endif
}

MatrixDS* parse_signature(unsigned char *salt_buffer) {
    MatrixDS* s = CreateMatrix(N, 1, false);

    #if RAINBOW_VERSION == 1
        unsigned char byte_read;

        for (unsigned short i = 0; i < n_variables; i+=2) {
            fill_buffer_randomly(&byte_read, 1);

            // inverted order
            set(s, i, 0, (gf) (byte_read & 0xf));
            set(s, i+1, 0, (gf) (byte_read >> 4));
        }

    #else
        for (unsigned short i = 0; i < n_variables; i++) {
            unsigned char byte_read;
            fill_buffer_randomly(&byte_read, 1);

            set(s, i, 0, (gf) byte_read);
        }

    #endif

    // read salt
    fill_buffer_randomly(salt_buffer, SALT_SIZE);
    
    // compute the quadratic products
    unsigned int h = N - 1;

    for (unsigned int i = n_variables; i > 0; i--)
        for (unsigned int j = n_variables; j >= i; j--)
            set(s, h--, 0, GF_MUL(get(s, i-1, 0), get(s, j-1, 0)));

    return s;
}

/*MatrixDS* parse_signature(unsigned char *salt_buffer) {
    MatrixDS* s = CreateMatrix(N, 1, false);

    send_ok();

    #if RAINBOW_VERSION == 1
        unsigned char byte_read;

        for (unsigned short i = 0; i < n_variables; i+=2) {
            byte_read = read_byte();

            // inverted order
            set(s, i, 0, (gf) (byte_read & 0xf));
            set(s, i+1, 0, (gf) (byte_read >> 4));
        }

    #else
        for (unsigned short i = 0; i < n_variables; i++)
            set(s, i, 0, (gf) read_byte());

    #endif

    // read salt
    read_n_bytes(salt_buffer, SALT_SIZE);
    
    // compute the quadratic products
    unsigned int h = N - 1;

    for (unsigned int i = n_variables; i > 0; i--)
        for (unsigned int j = n_variables; j >= i; j--)
            set(s, h--, 0, GF_MUL(get(s, i-1, 0), get(s, j-1, 0)));

    return s;
}*/

MatrixDS* get_result_vector(const unsigned char* digest) {
    MatrixDS* u = CreateMatrix(n_polynomials, 1, false);

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
