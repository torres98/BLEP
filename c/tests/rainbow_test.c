#include <stdio.h>
#include <stdlib.h>

#include "blep/schemes/rainbow.h"
#include "blep/utils/rand.h"
#include "blep/mv_verification/std_ver.h"
#include "blep/mv_verification/eff_ver.h"
#include "blep/mv_verification/prog_ver.h"

#if RAINBOW_VERSION == 1
    #define RAINBOW_VERSION_STR "I"
    #ifdef GF16_LOOKUP
        #define GF_LOOKUP GF16_LOOKUP
    #else
        #define GF_LOOKUP 0
    #endif
#elif RAINBOW_VERSION == 2
    #define RAINBOW_VERSION_STR "III"
    #ifdef GF256_LOOKUP
        #define GF_LOOKUP GF256_LOOKUP
    #else
        #define GF_LOOKUP 0
    #endif
#else
    #define RAINBOW_VERSION_STR "V"
    #ifdef GF256_LOOKUP
        #define GF_LOOKUP GF256_LOOKUP
    #else
        #define GF_LOOKUP 0
    #endif
#endif

#define assert(e) if ((e) == 0) return 1
#define STR_IMPL_(x) #x
#define STR(x) STR_IMPL_(x)

char const *pk_path = STR(PROJECT_DIR) "/../rainbow_examples/pk" STR(RAINBOW_VERSION) ".txt";
char const *signature_path = STR(PROJECT_DIR) "/../rainbow_examples/signature" STR(RAINBOW_VERSION) ".txt";
char const *message_path = STR(PROJECT_DIR) "/../rainbow_examples/message.txt";

unsigned char salt[SALT_SIZE];

int main() {
    MatrixDS* PK = get_public_key_from_file(pk_path);
    MatrixDS* s = get_signature_from_file(signature_path, salt);
    MatrixDS* u = get_result_vector(message_path, salt);
    
    printf("Rainbow " RAINBOW_VERSION_STR " (lookup level " STR(GF_LOOKUP) ")\n");

    assert(verify_signature_with_result_vector(PK, s, u));
    printf("Successfully verified the Rainbow signature.\n");
    
    // for efficient verification
    uint16_t k = n_polynomials / 2;
    MatrixDS *C = CreateMatrix(k, nrows(PK), false);
    MatrixDS *SVK = offVer(PK, C, k);
    MatrixDS *u_eff = dot_product(C, u);

    assert(verify_signature_with_result_vector(SVK, s, u_eff));
    printf("Efficiently verified the Rainbow signature.\n");

    // for progressive verification
    uint16_t t = n_polynomials / 2;

    assert(progVer_with_result_vector(PK, s, u, t, true) == true);
    printf("Progressively verified the Rainbow signature.\n");

    assert(progVer_with_result_vector(SVK, s, u_eff, t, true));
    printf("Efficiently and progressively verified the Rainbow signature.\n");

    destroy_matrix(PK);
    destroy_matrix(s);
    destroy_matrix(u);
    
    destroy_matrix(C);
    destroy_matrix(SVK);
    destroy_matrix(u_eff);

    return 0;
}
