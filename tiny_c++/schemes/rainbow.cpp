#include <device.h>
#include <drivers/uart.h> 

#include "../include/uart_utils.h"
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

        constexpr void (*hash_raw_str)(unsigned char* raw_str, unsigned char* output_buffer, unsigned long long mlen) = sha256_raw_str;
        constexpr void (*hash_from_device)(const struct device* dev, unsigned char* output_buffer, unsigned long long mlen) = sha256_from_device;

        typedef gf16 gf;
    #elif VERSION == 2
        const unsigned short v1 = 68, o1 = 32, o2 = 48, q = 256, element_hex_size = 2, DIGEST_SIZE = 48;

        constexpr void (*hash_raw_str)(unsigned char* raw_str, unsigned char* output_buffer, unsigned long long mlen) = sha384_raw_str;
        constexpr void (*hash_from_device)(const struct device* dev, unsigned char* output_buffer, unsigned long long mlen) = sha384_from_device;

        typedef gf256 gf;
    #elif VERSION == 3
        const unsigned short v1 = 96, o1 = 36, o2 = 64, q = 256, element_hex_size = 2, DIGEST_SIZE = 64;

        constexpr void (*hash_raw_str)(unsigned char* raw_str, unsigned char* output_buffer, unsigned long long mlen) = sha512_raw_str;
        constexpr void (*hash_from_device)(const struct device* dev, unsigned char* output_buffer, unsigned long long mlen) = sha512_from_device;

        typedef gf256 gf;
    #endif
    
    const unsigned int n_variables = v1+o1+o2, n_polynomials = n_variables - v1, N = n_variables * (n_variables + 1) / 2, elements_per_byte = 2 / element_hex_size, SALT_SIZE = 16, remaining_bytes = n_polynomials - DIGEST_SIZE * elements_per_byte;

    Vector<gf> parse_signature(const struct device *uart_dev, unsigned char *salt_buffer) {

        Vector<gf> s = Vector<gf>(N);

        if (element_hex_size == 1) {
            unsigned char byte_read;

            for (unsigned int i = 0; i < n_variables; i+=2) {
                byte_read = read_byte(uart_dev);

                // inverted order
                s[i] = gf(byte_read & 0xf);
                s[i+1] = gf(byte_read >> 4);
            }

        } else {
            for (unsigned int i = 0; i < n_variables; i++)
                s[i] = gf(read_byte(uart_dev));
        }

        // read salt
        read_n_bytes(uart_dev, salt_buffer, SALT_SIZE);
        
        // compute the quadratic products
        unsigned int h = N - 1;

        for (unsigned int i = n_variables; i > 0; i--)
            for (unsigned int j = n_variables; j >= i; j--)
                s[h--] = s[i-1] * s[j-1];

        return s;

    }

    void get_message_digest(const struct device *uart_dev, unsigned char *output_buffer, unsigned int mlen) {
        hash_from_device(uart_dev, output_buffer, mlen);
    }

    void get_complete_digest(unsigned char *output_buffer, const unsigned char *message_digest, const unsigned char* salt) {
        unsigned char temp_buffer[DIGEST_SIZE * 2];

        // temp_buffer = H(m) || salt
        memcpy(temp_buffer, message_digest, DIGEST_SIZE);
        memcpy(temp_buffer + DIGEST_SIZE, salt, SALT_SIZE);
        
        hash_raw_str(temp_buffer, output_buffer, DIGEST_SIZE + SALT_SIZE);

        if (remaining_bytes != 0) {
            hash_raw_str(output_buffer, temp_buffer, DIGEST_SIZE);
            memcpy(output_buffer + DIGEST_SIZE, temp_buffer, remaining_bytes);
        }

    }

    void get_complete_digest(const struct device *uart_dev, unsigned char *output_buffer, const unsigned char* salt, unsigned int mlen) {
        unsigned char temp_buffer[DIGEST_SIZE * 2];

        get_message_digest(uart_dev, temp_buffer, mlen);
        
        // copy the salt at the end of the array (digest = H(m) || salt)
        memcpy(temp_buffer + DIGEST_SIZE, salt, SALT_SIZE);

        hash_raw_str(temp_buffer, output_buffer, DIGEST_SIZE + SALT_SIZE);

        if (remaining_bytes != 0) {
            hash_raw_str(output_buffer, temp_buffer, DIGEST_SIZE);
            memcpy(output_buffer + DIGEST_SIZE, temp_buffer, remaining_bytes);
        }

    }

    Vector<gf> get_result_vector(const unsigned char* digest) {
        Vector<gf> u = Vector<gf>(n_polynomials);

        // insert the final digest
        if (element_hex_size == 1) {
            for (unsigned int i = 0; i < n_polynomials; i+=2) {
                u[i] = gf(digest[i/2] >> 4);
                u[i+1] = gf(digest[i/2] & 0xf);
            }
        } else {
            for (unsigned int i = 0; i < n_polynomials; i++)
                u[i] = gf(digest[i]);
        }

        return u;
    }

};
