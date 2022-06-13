#include <cstdio>

#include "../include/rainbow.h"

#include "../include/hash_utils.h"
#include "../include/matrix.h"
#include "../include/vector.h"

#define hex_to_int(chr) (chr >= 'a') ? (chr - 'a' + 10) : (chr - '0')

#if VERSION == 1
    constexpr void (*hash_str)(const unsigned char* message, unsigned char* output_buffer, unsigned long long mlen) = sha256_string;
    constexpr int (*hash_file)(const char* file_path, unsigned char* output_buffer) = sha256_file;

#elif VERSION == 2
    constexpr void (*hash_str)(const unsigned char* message, unsigned char* output_buffer, unsigned long long mlen) = sha384_string;
    constexpr int (*hash_file)(const char* file_path, unsigned char* output_buffer) = sha384_file;

#elif VERSION == 3
    constexpr void (*hash_str)(const unsigned char* message, unsigned char* output_buffer, unsigned long long mlen) = sha512_string;
    constexpr int (*hash_file)(const char* file_path, unsigned char* output_buffer) = sha512_file;

#endif


Matrix<Rainbow::gf> Rainbow::parse_public_key(const char *pk_path) {
    Matrix<gf> PK = Matrix<gf>(n_polynomials, N, true);
    FILE *pk_file = fopen(pk_path, "r");

    //consume the header of the pk file
    while (fgetc(pk_file) != '=');
    fgetc(pk_file);

    char element_hex[element_hex_size + 1];

    for (unsigned int j=0; j < N; j++) {
        for (unsigned int i=0; i < n_polynomials; i++) {
            fgets(element_hex, element_hex_size + 1, pk_file);
            PK(i, j) = gf(strtol(element_hex, NULL, 16)); 
        }
    }

    fclose(pk_file);

    return PK;
}

Vector<Rainbow::gf> Rainbow::parse_signature(const char* signature_path, unsigned char *salt_buffer) {

    Vector<gf> s = Vector<gf>(N);

    FILE *signature_file = fopen(signature_path, "r");

    // consume signature header
    while (fgetc(signature_file) != '=');
    fgetc(signature_file);

    char buffer[3];

    // fill the signature vector
    if (element_hex_size == 1) {
        for (unsigned int i = 0; i < n_variables; i+=2) {
            fgets(buffer, 3, signature_file);
            s(i) = gf(hex_to_int(buffer[1]));
            s(i+1) = gf(hex_to_int(buffer[0]));
        }
    } else {
        for (unsigned int i = 0; i < n_variables; i++) {
            fgets(buffer, 3, signature_file);
            s(i) = gf(strtol(buffer, NULL, 16));
        }
    }

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
            s(h--) = s(i-1) * s(j-1);

    return s;

}

void Rainbow::get_message_digest(const char* message_path, unsigned char *output_buffer) {
    hash_file(message_path, output_buffer);
}
    
void Rainbow::get_complete_digest(const char* message_path, const unsigned char* salt, unsigned char *output_buffer) {
    unsigned char temp_buffer[DIGEST_SIZE + SALT_SIZE];

    // temp_buffer = H(m)
    get_message_digest(message_path, temp_buffer);

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

Vector<Rainbow::gf> Rainbow::get_result_vector(const char* message_path, const unsigned char* salt) {
    Vector<gf> u = Vector<gf>(n_polynomials);

    unsigned char digest[n_polynomials]; //fix this

    get_complete_digest(message_path, salt, digest);

    // insert the final digest
    if (element_hex_size == 1) {
        for (unsigned int i = 0; i < n_polynomials; i+=2) {
            u(i) = gf(digest[i/2] >> 4);
            u(i+1) = gf(digest[i/2] & 0xf);
        }
    } else {
        for (unsigned int i = 0; i < n_polynomials; i++)
            u(i) = gf(digest[i]);
    }

    return u;
}
