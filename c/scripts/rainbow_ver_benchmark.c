#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "blep/utils/rand.h"
#include "blep/schemes/rainbow.h"
#include "blep/mv_verification/std_ver.h"
#include "blep/mv_verification/eff_ver.h"
#include "blep/mv_verification/prog_ver.h"

#if RAINBOW_VERSION == 1
    #define RAINBOW_VERSION_STR "I"
    #define GF_LOOKUP GF16_LOOKUP
#elif RAINBOW_VERSION == 2
    #define RAINBOW_VERSION_STR "III"
    #define GF_LOOKUP GF256_LOOKUP
#else
    #define RAINBOW_VERSION_STR "V"
    #define GF_LOOKUP GF256_LOOKUP
#endif

#define CLOCK_TO_AVG_MICROSECONDS(clocks, n) (clocks * (1000000.0L / n)) / CLOCKS_PER_SEC
#define STR_IMPL_(x) #x
#define STR(x) STR_IMPL_(x)

char const *pk_path = STR(PROJECT_DIR) "/../rainbow_examples/pk" STR(RAINBOW_VERSION) ".txt";
char const *signature_path = STR(PROJECT_DIR) "/../rainbow_examples/signature" STR(RAINBOW_VERSION) ".txt";
char const *message_path = STR(PROJECT_DIR) "/../rainbow_examples/message.txt";

unsigned char salt[SALT_SIZE];

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Wrong number of arguments.\n");
        return -1;
    }

    uint16_t k = strtoul(argv[1], NULL, 10), t = strtoul(argv[2], NULL, 10);
    uint32_t SAMPLE_SIZE = strtoul(argv[3], NULL, 10);
    
    if (k > n_polynomials) {
        fprintf(stderr, "The number of rows of the SVK (%u) can't be bigger than the number of rows of the PK (%u)\n", k, n_polynomials);
        return -1;
    } else if (t > k) {
        fprintf(stderr, "The number of progressive steps (%u) can't be bigger than the number of rows of the SVK (%u)\n", t, k);
        return -1;
    }

    //srand((unsigned int) time(NULL)); fix

    MatrixDS* PK = get_public_key_from_file(pk_path);
    MatrixDS* s = get_signature_from_file(signature_path, salt);

    MatrixDS* u = get_result_vector(message_path, salt);
    
    // for efficient verification
    MatrixDS *C = CreateMatrix(k, nrows(PK), false);
    MatrixDS *svk = offVer(PK, C, k);
    MatrixDS *u_eff = dot_product(C, u);

    MatrixDS* signature_guess = CreateMatrix(nrows(s), 1, false);
    uint32_t eff_error_count = 0, prog_error_count = 0, effprog_error_count = 0;

    clock_t eff_clocks = 0, prog_clocks = 0, effprog_clocks = 0;

    for (uint32_t i = 0; i < SAMPLE_SIZE; i++) {
        fill_matrix_randomly(signature_guess);

        clock_t starting_clock = clock();
        bool eff_verification_result = verify_signature_with_result_vector(svk, signature_guess, u_eff);
        eff_clocks += clock() - starting_clock;

        starting_clock = clock();
        bool prog_verification_result = progVer_with_result_vector(PK, signature_guess, u, t, true);
        prog_clocks += clock() - starting_clock;

        starting_clock = clock();
        bool effprog_verification_result = progVer_with_result_vector(svk, signature_guess, u_eff, t, true);
        effprog_clocks += clock() - starting_clock;

        if (eff_verification_result || prog_verification_result || effprog_verification_result) {
            if (!verify_signature_with_result_vector(PK, signature_guess, u)) {
                if (eff_verification_result)
                    eff_error_count++;
                
                if (prog_verification_result)
                    prog_error_count++;

                if (effprog_verification_result)
                    effprog_error_count++;
            }
        }
    }

    printf("Rainbow " RAINBOW_VERSION_STR " (lookup level " STR(GF_LOOKUP) ")\n");

    printf("EFFICIENT VERIFICATION STATS (%u SVK rows)\n", k);
    printf("Error percentage: %.4Lf%%\n", (long double) eff_error_count / SAMPLE_SIZE * 100);
    printf("Security bits: %.4f\n", ((eff_error_count == 0) ? 0: log2((long double) eff_error_count / SAMPLE_SIZE)));
    printf("Average verification time: %.0Lf microseconds\n\n", CLOCK_TO_AVG_MICROSECONDS(eff_clocks, SAMPLE_SIZE));

    printf("PROGRESSIVE VERIFICATION STATS (%u progressive steps)\n", t);
    printf("Error percentage: %.4Lf%%\n", (long double) prog_error_count / SAMPLE_SIZE * 100);
    printf("Security bits: %.4f\n", ((prog_error_count == 0) ? 0: log2((long double) prog_error_count / SAMPLE_SIZE)));
    printf("Average verification time: %.0Lf microseconds\n\n", CLOCK_TO_AVG_MICROSECONDS(prog_clocks, SAMPLE_SIZE));

    printf("EFFICIENT+PROGRESSIVE VERIFICATION STATS (%u SVK rows, %u progressive steps)\n", k, t);
    printf("Error percentage: %.4Lf%%\n", (long double) effprog_error_count / SAMPLE_SIZE * 100);
    printf("Security bits: %.4f\n", ((effprog_error_count == 0) ? 0: log2((long double) effprog_error_count / SAMPLE_SIZE)));
    printf("Average verification time: %.0Lf microseconds\n", CLOCK_TO_AVG_MICROSECONDS(effprog_clocks, SAMPLE_SIZE));

    destroy_matrix(PK);
    destroy_matrix(s);
    destroy_matrix(u);
    
    destroy_matrix(C);
    destroy_matrix(svk);
    destroy_matrix(u_eff);
    destroy_matrix(signature_guess);

    return 0;
}
