#include <stdio.h>
#include <stdlib.h>

#include "blep/mv_verification/std_ver.h"


bool verify_signature(const MatrixDS* M, const MatrixDS* v) {
    if (ncolumns(M) != nrows(v)) {
        fprintf(stderr, "Incompatible sizes for signature verification (public key of size %ux%u and signature of size %u)", nrows(M), ncolumns(M), nrows(v));
        exit(-1);
    }

    bool verification_result = true;

    for (uint16_t i = 0; i < nrows(M); i++)
        if (row_vector_product(M, v, i))
            verification_result = false;

    return verification_result;
}

bool verify_signature_with_result_vector(const MatrixDS* PK, const MatrixDS* s, const MatrixDS* u) {
    if (ncolumns(PK) != nrows(s)) {
        fprintf(stderr, "Incompatible sizes for signature verification (public key of size %ux%u and signature of size %u)", nrows(PK), ncolumns(PK), nrows(s));
        exit(-1);
    } else if (nrows(PK) != nrows(u)) {
        fprintf(stderr, "Incompatible sizes for signature verification (public key of size %ux%u and resulting vector of size %u)", nrows(PK), ncolumns(PK), nrows(u));
        exit(-1);
    }

    bool verification_result = true;

    for (uint16_t i = 0; i < nrows(PK); i++)
        if (row_vector_product(PK, s, i) != get(u, i, 0))
            verification_result = false;

    return verification_result;
}
