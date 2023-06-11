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

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Expected 2 args: k (#svk rows) s (#runs).\n");
        return -1;
    }

    uint16_t k = strtoul(argv[1], NULL, 10);
    uint32_t SAMPLE_SIZE = strtoul(argv[2], NULL, 10);
    
    if (k > n_polynomials) {
        fprintf(stderr, "The number of rows of the SVK (%u) can't be bigger than the number of rows of the PK (%u)\n", k, n_polynomials);
        return -1;
    }

    MatrixDS* PK = get_public_key_from_file(pk_path);

    MatrixDS *C = CreateMatrix(k, nrows(PK), false);

    clock_t offver_clocks = 0;

    for (uint32_t i = 0; i < SAMPLE_SIZE; i++) {
        clock_t starting_clock = clock();
        MatrixDS *svk = offVer(PK, C, k);
        offver_clocks += clock() - starting_clock;
        destroy_matrix(svk);
    }

    printf("Rainbow " RAINBOW_VERSION_STR " (lookup level " STR(GF_LOOKUP) ")\n");
    printf("Average SVK generation time (%u SVK rows): %.0Lf microseconds\n", k, (long double) CLOCK_TO_AVG_MICROSECONDS(offver_clocks, SAMPLE_SIZE));

    destroy_matrix(PK);
    destroy_matrix(C);

    return 0;
}
