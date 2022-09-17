#include <iostream>
#include <iomanip>

#include <kernel.h>

#include "tiny/include/math_utils.h"
#include "tiny/include/progressive_verification.h"
#include "tiny/include/standard_verification.h"
#include "tiny/include/rainbow.h"
#include "tiny/include/uart_utils.h"

#include "svk.h"


using Rainbow::gf;

unsigned char salt[Rainbow::SALT_SIZE];
unsigned char message_digest[Rainbow::SHA_DIGEST_SIZE];
unsigned char final_digest[Rainbow::FINAL_DIGEST_SIZE];

int main() {
    // declare variables
    uint32_t result_vector_time = 0, eff_verification_time = 0, effprog_verification_time = 0;

    //Receive svk_nrows, progressive_steps and sample_size
    uint16_t PROGRESSIVE_STEPS = read_uint32(uart_dev), SAMPLE_SIZE = read_uint32(uart_dev);

    // read message length (4 bytes) 
    uint32_t mlen = read_uint32(uart_dev);
    
    Rainbow::get_message_digest(uart_dev, message_digest, mlen);

    // load the linear transformation and the short verification key as a Matrix objects
    MatrixDS<gf> SVK = MatrixDS<gf>((gf*) short_private_key, SVK_NROWS, Rainbow::N);
    MatrixDS<gf> C = MatrixDS<gf>((gf*) private_transformation, SVK_NROWS, Rainbow::n_polynomials);

    for (unsigned int i = 0; i < SAMPLE_SIZE; i++) {
        VectorDS<gf> s = Rainbow::parse_signature(uart_dev, salt);

        // Compute resulting vector
        Rainbow::get_complete_digest(final_digest, message_digest, salt);
        VectorDS<gf> u = Rainbow::get_result_vector(final_digest);

        // 1) Compute reduced resulting vector
        uint32_t starting_cycle = k_cycle_get_32();
        VectorDS<gf> u_eff = (VectorDS<gf>) (C * u);
        result_vector_time = k_cyc_to_us_floor32(k_cycle_get_32() - starting_cycle);

        // 2) Perform efficient verification
        starting_cycle = k_cycle_get_32();
        bool efficient_result = verify_signature(SVK, s, u_eff);
        eff_verification_time += k_cyc_to_us_floor32(k_cycle_get_32() - starting_cycle);

        // 3) Perform efficient+progressive verification
        starting_cycle = k_cycle_get_32();
        bool effprog_result = progVer(SVK, s, u_eff, PROGRESSIVE_STEPS);
        effprog_verification_time += k_cyc_to_us_floor32(k_cycle_get_32() - starting_cycle);

        std::cout << (unsigned char) efficient_result << (unsigned char) effprog_result << std::endl;
    }

    std::cout << std::fixed;
    std::cout << (double) result_vector_time / SAMPLE_SIZE << std::endl;
    std::cout << (double) eff_verification_time / SAMPLE_SIZE << std::endl;
    std::cout << (double) effprog_verification_time / SAMPLE_SIZE << std::endl;

    return 0;
}
