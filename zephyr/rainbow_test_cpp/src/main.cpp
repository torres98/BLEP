#include <iostream>
#include <iomanip>

#include <zephyr/kernel.h>

#include "tiny/include/math_utils.h"
#include "tiny/include/progressive_verification.h"
#include "tiny/include/standard_verification.h"
#include "tiny/include/rainbow.h"
#include "tiny/include/uart_utils.h"

#include "svk.h"


using Rainbow::gf;

unsigned char salt[Rainbow::SALT_SIZE];
unsigned char final_digest[Rainbow::FINAL_DIGEST_SIZE];

int main() {
    uint16_t PROGRESSIVE_STEPS = read_uint32(uart_dev);

    //read signature
    VectorDS<gf> s = Rainbow::parse_signature(uart_dev, salt);

    // read message length (4 bytes) 
    uint32_t mlen = read_uint32(uart_dev);
    
    // read message and compute its Rainbow digest
    Rainbow::get_complete_digest(uart_dev, final_digest, salt, mlen);

    std::cout << std::setfill('0') << std::hex;

    for (unsigned int i = 0; i < Rainbow::FINAL_DIGEST_SIZE; i++)
        std::cout << std::setw(2) << (unsigned int) final_digest[i];

    std::cout << std::endl;

    // load the linear transformation and the short verification key as a Matrix objects
    MatrixDS<gf> SVK = MatrixDS<gf>((gf*) short_private_key, SVK_NROWS, Rainbow::N);
    MatrixDS<gf> C = MatrixDS<gf>((gf*) private_transformation, SVK_NROWS, Rainbow::n_polynomials);
    
    // Compute reduced result vector
    VectorDS<gf> u_eff = VectorDS<gf>(C * Rainbow::get_result_vector(final_digest));

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
