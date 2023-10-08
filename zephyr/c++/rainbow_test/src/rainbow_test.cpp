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
unsigned char final_digest[Rainbow::FINAL_DIGEST_SIZE];

template <typename V>
void print_buffer(const V buffer, unsigned n, int width=2) {
    std::cout << std::setfill('0') << std::hex;

    for (unsigned int i = 0; i < n; i++)
        std::cout << std::setw(width) << (unsigned) buffer[i];

    std::cout << std::endl;
}

int main() {
    uint16_t PROGRESSIVE_STEPS = read_uint32();

    //read signature
    VectorDS<gf> s = Rainbow::parse_signature(salt);

    // read message length (4 bytes) 
    uint32_t mlen = read_uint32();

    // read message and compute its Rainbow digest
    Rainbow::get_complete_digest(salt, mlen, final_digest);

    print_buffer(final_digest, Rainbow::FINAL_DIGEST_SIZE);

    // load the linear transformation and the short verification key as a Matrix objects
    const MatrixDS<gf> SVK = MatrixDS<gf>((gf*) short_private_key, SVK_NROWS, Rainbow::N);
    const MatrixDS<gf> C = MatrixDS<gf>((gf*) private_transformation, SVK_NROWS, Rainbow::n_polynomials);
    
    // Compute reduced result vector
    VectorDS<gf> u_eff = C * Rainbow::get_result_vector(final_digest);

    // Perform efficient verification
    if (verify_signature(SVK, s, u_eff))
        std::cout << "Success" << std::endl;
    else
        std::cout << "Failure" << std::endl;

    // Perform efficient + progressive verification
    if (progVer(SVK, s, u_eff, PROGRESSIVE_STEPS))
        std::cout << "Success" << std::endl;
    else
        std::cout << "Failure" << std::endl;

    return 0;
}
