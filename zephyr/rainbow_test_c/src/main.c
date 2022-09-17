#include <stdio.h>

#include <kernel.h>

#include "tiny/include/standard_verification.h"
#include "tiny/include/progressive_verification.h"
#include "tiny/include/rainbow.h"
#include "tiny/include/math_utils.h"
#include "tiny/include/uart_utils.h"

#include "svk.h"


unsigned char salt[SALT_SIZE];
unsigned char final_digest[FINAL_DIGEST_SIZE];

int main() {
    uint16_t PROGRESSIVE_STEPS = read_uint32(uart_dev);

    //read signature
    MatrixDS* s = parse_signature(uart_dev, salt);

    // read message length (4 bytes) 
    uint32_t mlen = read_uint32(uart_dev);

    // read message and compute its Rainbow digest
    get_complete_digest_from_device(uart_dev, final_digest, salt, mlen);

    for (unsigned int i = 0; i < FINAL_DIGEST_SIZE; i++)
        printf("%02x", (unsigned int) final_digest[i]);

    printf("\n");

    // load the linear transformation and the short verification key as a Matrix objects
    MatrixDS* SVK = CreateMatrixFromArray((gf*) short_private_key, SVK_NROWS, N, false);
    MatrixDS* C = CreateMatrixFromArray((gf*) private_transformation, SVK_NROWS, n_polynomials, false);

    // Compute reduced result vector
    MatrixDS* u_eff = dot_product(C, get_result_vector(final_digest));

    // Perform efficient verification
    if (verify_signature_with_result_vector(SVK, s, u_eff))
        printf("Success\n");
    else
        printf("Failure\n");

    // Perform efficient + progressive verification
    if (progVer_with_result_vector(SVK, s, u_eff, PROGRESSIVE_STEPS, false))
        printf("Success\n");
    else
        printf("Failure\n");
    
    destroy_matrix(s);
    destroy_matrix(SVK);
    destroy_matrix(C);
    destroy_matrix(u_eff);

    return 0;

}
