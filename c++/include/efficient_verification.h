#ifndef EFFICIENT_VERIFICATION_H
#define EFFICIENT_VERIFICATION_H

#include "math_utils.h"

template <typename Element>
MatrixDS<Element> generate_random_linear_transformation(uint16_t rows, uint16_t columns) {
    MatrixDS<Element> C = MatrixDS<Element>(rows, columns); 
    fill_matrix_randomly(C);

    return C;
}

template <typename Element>
MatrixDS<Element> offVer(const MatrixDS<Element> &M, uint16_t k) {
    MatrixDS<Element> SVK;

    do {
        MatrixDS<Element> C = generate_random_linear_transformation<Element>(k, M.nrows());
        SVK = C * M;
    } while(!SVK.has_full_rank());

    return SVK;
}

#endif
