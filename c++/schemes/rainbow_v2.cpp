#include <cstdio>

#include "../include/hash_utils.h"
#include "../include/matrix.h"
#include "../include/vector.h"

#ifndef VERSION
    #define VERSION 3
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

        constexpr void (*hash_str)(unsigned char* message, unsigned char* output_buffer, unsigned long long mlen) = sha256_string;
        constexpr int (*hash_file)(const char* file_path, unsigned char* output_buffer) = sha256_file;

        typedef gf16 gf;
    #elif VERSION == 2
        const unsigned short v1 = 68, o1 = 32, o2 = 48, q = 256, element_hex_size = 2, DIGEST_SIZE = 48;

        constexpr void (*hash_str)(unsigned char* message, unsigned char* output_buffer, unsigned long long mlen) = sha384_string;
        constexpr int (*hash_file)(const char* file_path, unsigned char* output_buffer) = sha384_file;

        typedef gf256 gf;
    #elif VERSION == 3
        const unsigned short v1 = 96, o1 = 36, o2 = 64, q = 256, element_hex_size = 2, DIGEST_SIZE = 64;

        constexpr void (*hash_str)(unsigned char* message, unsigned char* output_buffer, unsigned long long mlen) = sha512_string;
        constexpr int (*hash_file)(const char* file_path, unsigned char* output_buffer) = sha512_file;

        typedef gf256 gf;
    #endif
    
    const unsigned int n_variables = v1+o1+o2, n_polynomials = n_variables - v1, N = n_variables * (n_variables + 1) / 2, elements_per_byte = 2 / element_hex_size, SALT_SIZE = 16, remaining_bytes = n_polynomials - DIGEST_SIZE * elements_per_byte;

    Matrix<gf> parse_public_key(const char *pk_path) {
        Matrix<gf> PK = Matrix<gf>(n_polynomials, N, true);
        FILE *pk_file = fopen(pk_path, "r");

        //consume the header of the pk file
        while (fgetc(pk_file) != '=');
        fgetc(pk_file);

        char element_hex[element_hex_size + 1];

        for (unsigned int i=0; i < N; i++) {
            for (unsigned int j=0; j < n_polynomials; j++) {
                fgets(element_hex, element_hex_size + 1, pk_file);
                PK[j][i] = gf(strtol(element_hex, NULL, 16)); 
            }
        }

        fclose(pk_file);

        return PK;
    }

    Vector<gf> parse_signature(const char* signature_path, unsigned char *salt_buffer) {

        Vector<gf> s = Vector<gf>(N);

        FILE *signature_file = fopen(signature_path, "r");

        // consume signature header
        while (fgetc(signature_file) != '=');
        fgetc(signature_file);

        char element_hex[3];

        // fill the signature vector
        for (unsigned int i = 0; i < n_variables; i++) {
            fgets(element_hex, element_hex_size + 1, signature_file);
            s[i] = gf(strtol(element_hex, NULL, 16));
        }

        // fill the salt buffer
        for (unsigned int i = 0; i < SALT_SIZE; i++) {
            fgets(element_hex, 3, signature_file);
            salt_buffer[i] = strtol(element_hex, NULL, 16);
        }

        fclose(signature_file);

        // Compute the explicit signature vector

        if (element_hex_size == 1) {
            gf tmp;

            for (unsigned int i = 0; i < n_variables; i+=2) {
                tmp = s[i+1];
                s[i+1] = s[i];
                s[i] = tmp;
            }
        }

        // quadratic products
        unsigned int h = N - 1;

        for (unsigned int i = n_variables; i > 0; i--)
            for (unsigned int j = n_variables; j >= i; j--)
                s[h--] = s[i-1] * s[j-1];

        return s;

    }

    void get_message_digest(const char* message_path, unsigned char* salt, unsigned char *output_buffer) {
        unsigned char temp_buffer[DIGEST_SIZE + SALT_SIZE];

        // temp_buffer = H(m)
        hash_file(message_path, temp_buffer);

        // copy the salt at the end of the array (digest = H(m) || salt)
        memcpy(temp_buffer + DIGEST_SIZE, salt, SALT_SIZE);

        // temp_buffer = H( H(m) || salt )
        hash_str(temp_buffer, output_buffer, DIGEST_SIZE + SALT_SIZE);

        // pad the digest with its digest (digestception!)
        // digest = H( H(m) || salt ) || H( H( H(m) || salt ) )
        if (remaining_bytes != 0) {
            hash_str(output_buffer, temp_buffer, DIGEST_SIZE);
            memcpy(output_buffer + DIGEST_SIZE, temp_buffer, remaining_bytes);
        }
    }

    Vector<gf> get_result_vector(const char* message_path, unsigned char* salt) {
        Vector<gf> u = Vector<gf>(n_polynomials);

        unsigned char digest[n_polynomials]; //fix this

        get_message_digest(message_path, salt, digest);

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
