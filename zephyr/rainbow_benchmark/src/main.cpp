#include <iostream>
#include <iomanip>

#include <kernel.h>

#include "tiny/include/standard_verification.h"
#include "tiny/include/progressive_verification.h"
#include "tiny/schemes/rainbow.cpp"
#include "tiny/include/matrix.h"
#include "tiny/include/vector.h"
#include "tiny/include/uart_utils.h"

#include "svk.h"

using std::cout;
using std::endl;

using Rainbow::gf;

unsigned char salt[Rainbow::SALT_SIZE];
unsigned char message_digest[Rainbow::DIGEST_SIZE];
unsigned char final_digest[Rainbow::n_polynomials];

Vector<gf> u_eff;

//#define PROGRESSIVE_STEPS 2

int main() {

    // load the linear transformation and the short verification key as a Matrix objects
    Matrix<gf> SVK = Matrix<gf>((gf*) short_private_key, SVK_NROWS, Rainbow::N);
    Matrix<gf> C = Matrix<gf>((gf*) private_transformation, SVK_NROWS, Rainbow::n_polynomials);

    // declare variables
    uint32_t start_time, effver_time = 0, effprogver_time = 0; // tip: maybe use 64 bits?

    // read message length (4 bytes) 
    uint32_t mlen = read_uint32(uart_dev);
    
    // I should read the message only once, then compute the digest at each signature received
    Rainbow::get_message_digest(uart_dev, message_digest, mlen);

    for (unsigned int i = 0; i < SAMPLE_SIZE; i++) {

        Vector<gf> s = Rainbow::parse_signature(uart_dev, salt);

        // compute digest
        Rainbow::get_complete_digest(final_digest, message_digest, salt);
        u_eff = (Vector<gf>) (C * Rainbow::get_result_vector(final_digest));


        // perform efficient verification
        start_time = k_cycle_get_32();
        bool efficient_result = verify_signature_lazy(SVK, s, u_eff);
        effver_time += k_cycle_get_32() - start_time;


        // perform efficient+progressive verification
        start_time = k_cycle_get_32();
        bool effprog_result = progVer_lazy(SVK, s, u_eff, PROGRESSIVE_STEPS);
        effprogver_time += k_cycle_get_32() - start_time;

        std::cout << (unsigned char) efficient_result << (unsigned char) effprog_result << std::endl;

    }

    std::cout << "Average efficient verification time: " << (double) effver_time / SAMPLE_SIZE << " microseconds." << std::endl;
    std::cout << "Average efficient+progressive verification time: " << (double) effprogver_time / SAMPLE_SIZE << " microseconds." << std::endl;

    return 0;

}
