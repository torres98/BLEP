#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../include/random_utils.h"
#include "../include/rainbow.h"
#include "../include/standard_verification.h"
#include "../include/efficient_verification.h"
#include "../include/progressive_verification.h"

#define STR_IMPL_(x) #x
#define STR(x) STR_IMPL_(x)

char const *pk_path = "/home/torres/Desktop/Thesis/verification_implementation/tmp/pk" STR(RAINBOW_VERSION) ".txt";
char const *signature_path = "/home/torres/Desktop/Thesis/verification_implementation/tmp/signature"  STR(RAINBOW_VERSION) ".txt";
char const *message_path = "/home/torres/Desktop/Thesis/verification_implementation/tmp/debug.gdb";

int main(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(stderr, "Wrong number of arguments.\n");
        exit(-1);
    }

    uint16_t k = strtoul(argv[1], NULL, 10);
    uint16_t t = strtoul(argv[2], NULL, 10);
    
    if (k > n_polynomials) {
        fprintf(stderr, "The number of rows of the SVK (%u) can't be bigger than the number of rows of the PK (%u)\n", k, n_polynomials);
        return -1;
    } else if (t > k) {
        fprintf(stderr, "The number of progressive steps (%u) can't be bigger than the number of rows of the SVK (%u)\n", t, k);
        return -1;
    }

    //srand((unsigned int) time(NULL)); maybe put it back if needed

    unsigned char salt[SALT_SIZE];

    MatrixDS* PK = parse_public_key(pk_path);
    MatrixDS* s = parse_signature(signature_path, salt);
    MatrixDS* u = get_result_vector(message_path, salt);
    
    assert(verify_signature_with_result_vector(PK, s, u));

    printf("Successfully verified the Rainbow signature.\n");
    
    // for efficient verification
    MatrixDS *C = CreateMatrix(k, nrows(PK), false);
    MatrixDS *SVK = offVer(PK, C, k);
    MatrixDS *u_eff = dot_product(C, u);

    assert(verify_signature_with_result_vector(SVK, s, u_eff));
    printf("Efficiently verified the Rainbow signature.\n");

    // for progressive verification
    assert(progVer_with_result_vector(PK, s, u, t, true));
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
