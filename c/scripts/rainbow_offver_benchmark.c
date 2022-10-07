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


#define CLOCK_TO_AVG_MICROSECONDS(clocks, n) (clocks * (1000000.0L / n)) / CLOCKS_PER_SEC

#define STR_IMPL_(x) #x
#define STR(x) STR_IMPL_(x)

char const *pk_path = "/home/torres/Desktop/Thesis/verification_implementation/tmp/pk" STR(RAINBOW_VERSION) ".txt";
char const *signature_path = "/home/torres/Desktop/Thesis/verification_implementation/tmp/signature" STR(RAINBOW_VERSION) ".txt";
char const *message_path = "/home/torres/Desktop/Thesis/verification_implementation/tmp/debug.gdb";

unsigned char salt[SALT_SIZE];

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Wrong number of arguments.\n");
        return -1;
    }

    uint16_t k = strtoul(argv[1], NULL, 10);
    uint32_t SAMPLE_SIZE = strtoul(argv[2], NULL, 10);
    
    if (k > n_polynomials) {
        fprintf(stderr, "The number of rows of the SVK (%u) can't be bigger than the number of rows of the PK (%u)\n", k, n_polynomials);
        return -1;
    }

    MatrixDS* PK = parse_public_key(pk_path);

    MatrixDS *C = CreateMatrix(k, nrows(PK), false);

    clock_t offver_clocks = 0;

    for (uint32_t i = 0; i < SAMPLE_SIZE; i++) {
        clock_t starting_clock = clock();
        MatrixDS *svk = offVer(PK, C, k);
        offver_clocks += clock() - starting_clock;
        destroy_matrix(svk);
    }

    printf("Average offVer time (%u SVK rows): %Lf\n", k, (long double) CLOCK_TO_AVG_MICROSECONDS(offver_clocks, SAMPLE_SIZE));

    destroy_matrix(PK);
    destroy_matrix(C);

    return 0;
}
