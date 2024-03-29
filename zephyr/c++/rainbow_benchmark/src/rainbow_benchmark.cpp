#include <iostream>
#include <iomanip>

#include <zephyr/kernel.h>

#include "blep/math/matrix.h"
#include "blep/math/vector.h"
#include "blep/mv_verification/prog_ver.h"
#include "blep/mv_verification/std_ver.h"
#include "blep/schemes/rainbow.h"
#include "blep/utils/uart.h"

#include "svk.h"


using Rainbow::gf;

unsigned char salt[Rainbow::SALT_SIZE];
unsigned char message_digest[Rainbow::SHA_DIGEST_SIZE];
unsigned char final_digest[Rainbow::FINAL_DIGEST_SIZE];

int main() {
    // declare variables
    uint32_t result_vector_time = 0, eff_verification_time = 0, effprog_verification_time = 0;

    //Receive progressive_steps and sample_size
    uint16_t PROGRESSIVE_STEPS = read_uint32();
    uint16_t SAMPLE_SIZE = read_uint32();

    // read message length (4 bytes) 
    uint32_t mlen = read_uint32();
    
    Rainbow::get_message_digest(mlen, message_digest);

    // load the linear transformation and the short verification key as a Matrix objects
    const MatrixDS<gf> SVK = MatrixDS<gf>((gf*) short_private_key, SVK_NROWS, Rainbow::N);
    const MatrixDS<gf> C = MatrixDS<gf>((gf*) private_transformation, SVK_NROWS, Rainbow::n_polynomials);

    for (uint16_t i = 0; i < SAMPLE_SIZE; i++) {
        VectorDS<gf> s = Rainbow::parse_signature(salt);

        // Compute resulting vector
        Rainbow::get_complete_digest(final_digest, message_digest, salt);
        VectorDS<gf> u = Rainbow::get_result_vector(final_digest);

        // 1) Compute reduced resulting vector
        uint32_t starting_cycle = k_cycle_get_32();
        VectorDS<gf> u_eff = C * u;
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

    std::cout << std::fixed << std::setprecision(2);
    std::cout << (double) result_vector_time / SAMPLE_SIZE << std::endl;
    std::cout << (double) eff_verification_time / SAMPLE_SIZE << std::endl;
    std::cout << (double) effprog_verification_time / SAMPLE_SIZE << std::endl;

    return 0;
}
