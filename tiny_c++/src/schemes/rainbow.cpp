#include <cstring>

#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#include "blep/schemes/rainbow.h"

#include "blep/utils/hash.h"
#include "blep/math/matrix.h"
#include "blep/math/vector.h"
#include "blep/utils/rand.h"
#include "blep/utils/uart.h"

#if RAINBOW_VERSION == 1
    #define HASH_RAW_STR sha256_raw_str
    #define HASH_FROM_DEVICE sha256_from_device

#elif RAINBOW_VERSION == 2
    #define HASH_RAW_STR sha384_raw_str
    #define HASH_FROM_DEVICE sha384_from_device

#elif RAINBOW_VERSION == 3
    #define HASH_RAW_STR sha512_raw_str
    #define HASH_FROM_DEVICE sha512_from_device

#endif

using Rainbow::gf;

void Rainbow::get_message_digest(const unsigned char *message, unsigned char *output_buffer, unsigned int mlen) {
    HASH_RAW_STR(message, output_buffer, mlen);
}

void Rainbow::get_message_digest(unsigned char *output_buffer, unsigned int mlen) {
    send_ok(); //maybe move inside function

    HASH_FROM_DEVICE(output_buffer, mlen);
}

void Rainbow::get_complete_digest(unsigned char *output_buffer, const unsigned char *message_digest, const unsigned char* salt) {
    unsigned char temp_buffer[SHA_DIGEST_SIZE + SALT_SIZE];

    // temp_buffer = H(m) || salt
    std::memcpy(temp_buffer, message_digest, SHA_DIGEST_SIZE);
    std::memcpy(temp_buffer + SHA_DIGEST_SIZE, salt, SALT_SIZE);
    
    HASH_RAW_STR(temp_buffer, output_buffer, SHA_DIGEST_SIZE + SALT_SIZE);

    #if RAINBOW_VERSION != 1
        HASH_RAW_STR(output_buffer, temp_buffer, SHA_DIGEST_SIZE);
        memcpy(output_buffer + SHA_DIGEST_SIZE, temp_buffer, n_polynomials - SHA_DIGEST_SIZE);
    #endif
}

void Rainbow::get_complete_digest(unsigned char *output_buffer, const unsigned char* salt, unsigned int mlen) {
    unsigned char temp_buffer[SHA_DIGEST_SIZE + SALT_SIZE];

    //get_message_digest(temp_buffer, mlen);
    
    // copy the salt at the end of the array (digest = H(m) || salt)
    memcpy(temp_buffer + SHA_DIGEST_SIZE, salt, SALT_SIZE);

    HASH_RAW_STR(temp_buffer, output_buffer, SHA_DIGEST_SIZE + SALT_SIZE);

    #if RAINBOW_VERSION != 1
        HASH_RAW_STR(output_buffer, temp_buffer, SHA_DIGEST_SIZE);
        memcpy(output_buffer + SHA_DIGEST_SIZE, temp_buffer, n_polynomials - SHA_DIGEST_SIZE);
    #endif
}

VectorDS<gf> Rainbow::parse_signature(unsigned char *salt_buffer) {
    VectorDS<gf> s = VectorDS<gf>(N);

    for (unsigned short i = 0; i < n_variables; i++)
        s.set(i, sys_rand32_get());

    // read salt
    sys_rand_get(salt_buffer, SALT_SIZE);

    // compute the quadratic products
    unsigned int h = N - 1;

    for (unsigned int i = n_variables; i > 0; i--) {
        for (unsigned int j = n_variables; j >= i; j--) {
            s.set(h--, s(i-1) * s(j-1));
        }
    }

    return s;
}

/*VectorDS<gf> Rainbow::parse_signature(unsigned char *salt_buffer) {
    VectorDS<gf> s = VectorDS<gf>(N);

    send_ok();

    #if RAINBOW_VERSION == 1
        for (unsigned short i = 0; i < n_variables/(2*SEGMENT_SIZE); i++) {
            receive_segment();

            // inverted order
            for (unsigned short h = 0; h < SEGMENT_SIZE; h++) {
                s.set(2*h, gf(uart_rx_buffer[h]));
                s.set(2*h + 1, gf(uart_rx_buffer[h] >> 4));
            }
        }

        unsigned short last_segment_size = (n_variables % SEGMENT_SIZE) / 2;
        receive_n_bytes(last_segment_size);

        for (unsigned short h = 0; h < last_segment_size; h++) {
            s.set(2*h + (n_variables - 2*last_segment_size), gf(uart_rx_buffer[h]));
            s.set(2*h + 1 + (n_variables - 2*last_segment_size), gf(uart_rx_buffer[h] >> 4));
        }

    #else
        for (unsigned short i = 0; i < n_variables/SEGMENT_SIZE; i++) {
            receive_segment();

            // inverted order
            for (unsigned short h = 0; h < SEGMENT_SIZE; h++)
                s.set(h, gf(uart_rx_buffer[h]));
        }
    
        unsigned short last_segment_size = n_variables % SEGMENT_SIZE;
        receive_n_bytes(last_segment_size);

        for (unsigned short h = 0; h < last_segment_size; h++)
            s.set(h + (n_variables - last_segment_size), gf(uart_rx_buffer[h]));
    
    #endif

    // read salt
    read_n_bytes(salt_buffer, SALT_SIZE);

    // compute the quadratic products
    unsigned int h = N - 1;

    for (unsigned int i = n_variables; i > 0; i--)
        for (unsigned int j = n_variables; j >= i; j--)
            s.set(h--, s(i-1) * s(j-1));

    return s;
}*/

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
