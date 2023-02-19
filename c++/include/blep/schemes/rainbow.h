#ifndef RAINBOW_H
#define RAINBOW_H

#include <cstdio>

#include "blep/utils/hash.h"
#include "blep/math/matrix.h"
#include "blep/math/vector.h"

#ifndef RAINBOW_VERSION
    #define RAINBOW_VERSION 1
#endif

#if RAINBOW_VERSION == 1
    #include "blep/math/gf16.h"
#elif RAINBOW_VERSION == 2 || RAINBOW_VERSION == 3
    #include "blep/math/gf256.h"
#else
    #error "Rainbow version not supported."
#endif


namespace Rainbow {
    #if RAINBOW_VERSION == 1
        const unsigned short v1 = 36, o1 = 32, o2 = 32, q = 16;
        typedef gf16 gf;

    #elif RAINBOW_VERSION == 2
        const unsigned short v1 = 68, o1 = 32, o2 = 48, q = 256;
        typedef gf256 gf;

    #elif RAINBOW_VERSION == 3
        const unsigned short v1 = 96, o1 = 36, o2 = 64, q = 256;
        typedef gf256 gf;

    #endif

    const unsigned short n_variables = v1 + o1 + o2, n_polynomials = n_variables - v1, SALT_SIZE = 16;
    const unsigned int N = n_variables * (n_variables + 1) / 2;

    void get_message_digest(const unsigned char* message, size_t mlen, const unsigned char* salt, unsigned char *output_buffer);
    void get_message_digest_from_file(const char* message_path, const unsigned char* salt, unsigned char *output_buffer);

    /**
     * Get the Rainbow public key in matrix form from an array.
     * 
     * @param pk Pointer to an array containing the Rainbow public key informations.
     * 
     * @return MatrixDS object containing the Rainbow public key.
     */
    MatrixDS<gf> get_public_key(const unsigned char *pk);

    /**
     * Get the Rainbow public key in matrix form from a file.
     * 
     * @param pk_path Path to the Rainbow public key file.
     * 
     * @return MatrixDS object containing the Rainbow public key.
     * 
     * @throw Throws a std::runtime_error exception if the file is not properly formatted
     *        or a reading error occurs.
     */
    MatrixDS<gf> get_public_key_from_file(const char *pk_path);


    /**
     * Get the Rainbow signature in vector form from an array.
     * 
     * @param signature Pointer to an array containing the Rainbow signature informations.
     * 
     * @return VectorDS object containing the Rainbow signature.
     */
    VectorDS<gf> get_signature(const unsigned char* signature);

    /**
     * Get the Rainbow signature in vector form from a file and fill the given salt buffer.
     * 
     * @param signature Path to the Rainbow signature file.
     * @param salt_buffer Pointer to the salt buffer.
     * 
     * @return VectorDS object containing the Rainbow signature.
     * 
     * @throw Throws a std::runtime_error exception if the file is not properly formatted
     *        or a reading error occurs.
     */
    VectorDS<gf> get_signature_from_file(const char* signature_path, unsigned char *salt_buffer);

    /**
     * Get the Rainbow result vector from an array.
     * 
     * @param message Pointer to an array containing the signed message.
     * @param mlen Message length.
     * @param salt Pointer to an array containing the salt.
     * 
     * @return VectorDS object containing the Rainbow result vector.
     */
    VectorDS<gf> get_result_vector(const unsigned char* message, size_t mlen, const unsigned char* salt);

    /**
     * Get the Rainbow result vector from a file.
     * 
     * @param message_path Path to the signed message.
     * @param salt Pointer to an array containing the salt.
     * 
     * @return VectorDS object containing the Rainbow result vector.
     */
    VectorDS<gf> get_result_vector_from_file(const char* message_path, const unsigned char* salt);
};

#endif
