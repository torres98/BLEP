#include <device.h>
#include <drivers/uart.h> 

#include "../include/rainbow.h"

#include "../include/uart_utils.h"
#include "../include/hash_utils.h"
#include "../include/math_utils.h"

#if RAINBOW_VERSION == 1
    #define GF_MUL gf16_mul

    #define HASH_RAW_STR sha256_raw_str
    #define HASH_FROM_DEVICE sha256_from_device

#elif RAINBOW_VERSION == 2
    #define GF_MUL gf256_mul

    #define HASH_RAW_STR sha384_raw_str
    #define HASH_FROM_DEVICE sha384_from_device

#elif RAINBOW_VERSION == 3
    #define GF_MUL gf256_mul

    #define HASH_RAW_STR sha512_raw_str
    #define HASH_FROM_DEVICE sha512_from_device

#endif

using Rainbow::gf;


void Rainbow::get_message_digest(const struct device *uart_dev, unsigned char *output_buffer, unsigned int mlen) {
    send_ok(uart_dev); //maybe move inside function

    HASH_FROM_DEVICE(uart_dev, output_buffer, mlen);
}

void Rainbow::get_complete_digest(unsigned char *output_buffer, const unsigned char *message_digest, const unsigned char* salt) {
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

void Rainbow::get_complete_digest(const struct device *uart_dev, unsigned char *output_buffer, const unsigned char* salt, unsigned int mlen) {
    unsigned char temp_buffer[SHA_DIGEST_SIZE + SALT_SIZE];

    get_message_digest(uart_dev, temp_buffer, mlen);
    
    // copy the salt at the end of the array (digest = H(m) || salt)
    memcpy(temp_buffer + SHA_DIGEST_SIZE, salt, SALT_SIZE);

    HASH_RAW_STR(temp_buffer, output_buffer, SHA_DIGEST_SIZE + SALT_SIZE);

    #if RAINBOW_VERSION != 1
        HASH_RAW_STR(output_buffer, temp_buffer, SHA_DIGEST_SIZE);
        memcpy(output_buffer + SHA_DIGEST_SIZE, temp_buffer, n_polynomials - SHA_DIGEST_SIZE);
    #endif
}

VectorDS<gf> Rainbow::parse_signature(const struct device *uart_dev, unsigned char *salt_buffer) {
    VectorDS<gf> s = VectorDS<gf>(N);

    send_ok(uart_dev);

    #if RAINBOW_VERSION == 1
        for (unsigned int i = 0; i < n_variables; i+=2) {
            unsigned char byte_read = read_byte(uart_dev);

            // inverted order
            s.set(i, gf(byte_read));
            s.set(i+1, gf(byte_read >> 4));
        }

    #else
        //read_n_bytes_segmented(uart_dev, (unsigned char*) &(s(0)), n_variables);
        for (unsigned int i = 0; i < n_variables; i++)
            s.set(i, gf(read_byte(uart_dev)));
    
    #endif

    // read salt
    read_n_bytes(uart_dev, salt_buffer, SALT_SIZE);
    
    // compute the quadratic products
    unsigned int h = N - 1;

    for (unsigned int i = n_variables; i > 0; i--)
        for (unsigned int j = n_variables; j >= i; j--)
            s.set(h--, s(i-1) * s(j-1));

    return s;
}

VectorDS<gf> Rainbow::get_result_vector(const unsigned char* digest) {
    VectorDS<gf> u = VectorDS<gf>(n_polynomials);

    // insert the final digest
    #if RAINBOW_VERSION == 1
        for (unsigned short i = 0; i < n_polynomials; i+=2) {
            u.set(i, gf(digest[i/2] >> 4));
            u.set(i+1, gf(digest[i/2]));
        }

    #else
        for (unsigned short i = 0; i < n_polynomials; i++)
            u.set(i, gf(digest[i]));
        
    #endif

    return u;
}
