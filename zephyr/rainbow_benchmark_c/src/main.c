#include <stdio.h>

#include <zephyr/kernel.h>

#include "tiny/include/math_utils.h"
#include "tiny/include/progressive_verification.h"
#include "tiny/include/standard_verification.h"
#include "tiny/include/rainbow.h"
#include "tiny/include/uart_utils.h"

#include "svk.h"

unsigned char salt[SALT_SIZE];
unsigned char message_digest[SHA_DIGEST_SIZE];
unsigned char final_digest[FINAL_DIGEST_SIZE];

int main() {
    // declare variables
    uint32_t result_vector_time = 0, eff_verification_time = 0, effprog_verification_time = 0;

    //Receive svk_nrows, progressive_steps and sample_size
    uint16_t PROGRESSIVE_STEPS = read_uint32(uart_dev), SAMPLE_SIZE = read_uint32(uart_dev);

    // read message length (4 bytes) 
    uint32_t mlen = read_uint32(uart_dev);
    
    get_message_digest(uart_dev, message_digest, mlen);

    // load the linear transformation and the short verification key as a Matrix objects
    MatrixDS* SVK = CreateMatrixFromArray((gf*) short_private_key, SVK_NROWS, N, false);
    MatrixDS* C = CreateMatrixFromArray((gf*) private_transformation, SVK_NROWS, n_polynomials, false);

    for (unsigned int i = 0; i < SAMPLE_SIZE; i++) {
        MatrixDS* s = parse_signature(uart_dev, salt);

        // Compute resulting vector
        get_complete_digest(final_digest, message_digest, salt);
        MatrixDS* u = get_result_vector(final_digest);

        // 1) Compute reduced resulting vector
        uint32_t starting_cycle = k_cycle_get_32();
        MatrixDS* u_eff = dot_product(C, u);
        result_vector_time = k_cyc_to_us_floor32(k_cycle_get_32() - starting_cycle);

        // 2) Perform efficient verification
        starting_cycle = k_cycle_get_32();
        bool efficient_result = verify_signature_with_result_vector(SVK, s, u_eff);
        eff_verification_time += k_cyc_to_us_floor32(k_cycle_get_32() - starting_cycle);

        // 3) Perform efficient+progressive verification
        starting_cycle = k_cycle_get_32();
        bool effprog_result = progVer_with_result_vector(SVK, s, u_eff, PROGRESSIVE_STEPS, true);
        effprog_verification_time += k_cyc_to_us_floor32(k_cycle_get_32() - starting_cycle);

        destroy_matrix(s);
        destroy_matrix(u);
        destroy_matrix(u_eff);

        printf("%c%c\n", efficient_result, effprog_result);
    }

    printf("%u\n%u\n%u\n", result_vector_time / SAMPLE_SIZE, eff_verification_time / SAMPLE_SIZE, effprog_verification_time / SAMPLE_SIZE);

    destroy_matrix(SVK);
    destroy_matrix(C);

    return 0;
}
