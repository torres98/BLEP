#include <stdio.h>
#include <string.h>

#include <zephyr/kernel.h>

#include "tiny/include/math_utils.h"
#include "tiny/include/progressive_verification.h"
#include "tiny/include/standard_verification.h"
#include "tiny/include/rainbow.h"

#include "svk.h"

unsigned char salt[SALT_SIZE];
unsigned char message_digest[SHA_DIGEST_SIZE];
unsigned char final_digest[FINAL_DIGEST_SIZE];

const unsigned char message[] = "Hello, my name is Slim Shady";

int main() {
    // declare variables
    uint32_t result_vector_time = 0, eff_verification_time = 0, effprog_verification_time = 0;

    //Replace with local function
    get_message_digest(message, message_digest, strlen(message));

    // load the linear transformation and the short verification key as a Matrix objects
    MatrixDS* SVK = CreateMatrixFromArray((gf*) short_private_key, SVK_NROWS, N, false);
    MatrixDS* C = CreateMatrixFromArray((gf*) private_transformation, SVK_NROWS, n_polynomials, false);

    for (unsigned int i = 0; i < SAMPLE_SIZE; i++) {
        //Replace
        MatrixDS* s = parse_signature(salt);

        // Compute resulting vector
        get_complete_digest(final_digest, message_digest, salt);
        MatrixDS* u = get_result_vector(final_digest);

        // 1) Compute reduced resulting vector
        uint32_t starting_cycle = k_cycle_get_32();
        MatrixDS* u_eff = dot_product(C, u);
        result_vector_time += k_cyc_to_us_floor32(k_cycle_get_32() - starting_cycle);

        // 2) Perform efficient verification
        starting_cycle = k_cycle_get_32();
        verify_signature_with_result_vector(SVK, s, u_eff);
        eff_verification_time += k_cyc_to_us_floor32(k_cycle_get_32() - starting_cycle);

        // 3) Perform efficient+progressive verification
        starting_cycle = k_cycle_get_32();
        progVer_with_result_vector(SVK, s, u_eff, PROGRESSIVE_STEPS, true);
        effprog_verification_time += k_cyc_to_us_floor32(k_cycle_get_32() - starting_cycle);

        destroy_matrix(s);
        destroy_matrix(u);
        destroy_matrix(u_eff);

        //printf("%c%c\n", efficient_result, effprog_result);
    }

    printf("Average resulting vector time: %u microseconds\n", result_vector_time / SAMPLE_SIZE);
    printf("Average efficient verification time: %u microseconds\n", eff_verification_time / SAMPLE_SIZE);
    printf("Average efficient+progressive verification time: %u microseconds\n", effprog_verification_time / SAMPLE_SIZE);

    destroy_matrix(SVK);
    destroy_matrix(C);

    return 0;
}
