#include "blep/mv_verification/eff_ver.h"


#define NULL 0

MatrixDS* offVer(const MatrixDS* M, MatrixDS* C, uint16_t k) {
    MatrixDS *SVK = NULL;
    bool erase_transformation = (C == NULL);

    if (erase_transformation)
        C = CreateMatrix(k, nrows(M), false);

    do {
        destroy_matrix(SVK);

        fill_matrix_randomly(C); 
        SVK = dot_product(C, M);
    } while(!has_full_row_rank(SVK));

    if (erase_transformation)
        destroy_matrix(C);

    return SVK;
}
