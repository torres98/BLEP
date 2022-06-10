#include "../gf/gf16.cpp"
#include "../gf/gf256.cpp"
#include "../include/hash_utils.h"
#include "../include/standard_verification.h"
#include "../include/efficient_verification.h"
#include "../include/progressive_verification.h"

#include "../include/matrix.h"
#include "../include/vector.h"


struct Constants {
    unsigned short const v1, o1, o2, q, element_hex_size, DIGEST_SIZE;

    const unsigned int n_variables = v1+o1+o2, n_polynomials = n_variables - v1, N = n_variables * (n_variables + 1) / 2, elements_per_byte = 2 / element_hex_size, SALT_SIZE = 16, remaining_bytes = n_polynomials - DIGEST_SIZE * elements_per_byte;
};

struct Functions {
    void (*hash_str)(unsigned char* message, unsigned char* output_buffer, unsigned long long mlen);
    int (*hash_file)(const char* file_path, unsigned char* output_buffer);
};

template <const Constants &C, typename gf>
Matrix<gf> extract_public_key(const char *pk_path) {
        Matrix<gf> PK = Matrix<gf>(C.n_polynomials, C.N + C.n_polynomials, true);
        FILE *pk_file = fopen(pk_path, "r");

        //consume the header of the pk file
        while (fgetc(pk_file) != '=');
        fgetc(pk_file);

        char element_hex[C.element_hex_size + 1];

        for (unsigned int i=0; i < C.N; i++) {
            for (unsigned int j=0; j < C.n_polynomials; j++) {
                fgets(element_hex, C.element_hex_size + 1, pk_file);
                PK[j][i] = gf(strtol(element_hex, NULL, 16)); 
            }
        }

        fclose(pk_file);

        // concatenate the identity matrix in the right part
        for (unsigned int i=0; i < C.n_polynomials; i++)
            PK[i][C.N + i] = gf(1);

        return PK;
    }

namespace RainbowI {

    const unsigned short v1 = 36, o1 = 32, o2 = 32, q = 16;

    namespace {
        const Constants c = {v1, o1, o2, q, .element_hex_size = 1, .DIGEST_SIZE = 32};
        Functions f = {.hash_str = sha256_string, .hash_file = sha256_file};
    }

    typedef gf16 gf;
    
    extern Matrix<gf> (*extract_public_key)(const char*);
    Matrix<gf> (*extract_public_key)(const char*) = extract_public_key;//<c, gf16>;

}

RainbowI::extract_public_key("yoo");
/*  
    static Vector<gf> get_signature_vector(const char* signature_path, const char* message_path) {

        // PARSE SIGNATURE

        Vector<gf> v = Vector<gf>(N + n_polynomials);
        FILE *signature_file = fopen(signature_path, "r");

        while (fgetc(signature_file) != '=');
        fgetc(signature_file);

        char element_hex[3];

        for (unsigned int i = 0; i < n_variables; i++) {
            fgets(element_hex, element_hex_size + 1, signature_file);
            v[i] = gf(strtol(element_hex, NULL, 16));
        }

        if (element_hex_size == 1) {
            gf tmp;

            for (unsigned int i = 0; i < n_variables; i+=2) {
                tmp = v[i+1];
                v[i+1] = v[i];
                v[i] = tmp;
            }
        }

        // compute the quadratic products
        unsigned int h = N - 1;

        for (unsigned int i = n_variables; i > 0; i--)
            for (unsigned int j = n_variables; j >= i; j--)
                v[h--] = v[i-1] * v[j-1];

        
        // PARSE MESSAGE

        // will contain the message digest and the salt
        unsigned char digest[DIGEST_SIZE * 2];

        // digest = H(m)
        hash_file(message_path, digest);

        // copy the salt at the end of the array (digest = H(m) || salt)
        for (unsigned int i = 0; i < SALT_SIZE; i++) {
            fgets(element_hex, 3, signature_file);
            digest[DIGEST_SIZE + i] = strtol(element_hex, NULL, 16);
        }

        fclose(signature_file);

        // digest = H( H(m) || salt )
        hash_str(digest, digest, DIGEST_SIZE + 16);

        // pad the digest with its digest (inception!)
        // digest = digest || H( digest )
        if (remaining_bytes != 0)
            hash_str(digest, digest + DIGEST_SIZE, DIGEST_SIZE);

        // insert the final digest
        if (element_hex_size == 1) {
            for (unsigned int i = 0; i < n_polynomials; i+=2) {
                v[N + i] = gf(digest[i/2] >> 4);
                v[N + (i+1)] = gf(digest[i/2] & 0xf);
            }
        } else {
            for (unsigned int i = 0; i < n_polynomials; i++)
                v[N + i] = gf(digest[i]);
        }

        return v;
    }
}*/
