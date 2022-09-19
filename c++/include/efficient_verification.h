#ifndef EFFICIENT_VERIFICATION_H
#define EFFICIENT_VERIFICATION_H

#include <tuple>

#include "math_utils.h"


template <typename Element>
std::tuple<MatrixDS<Element>, MatrixDS<Element>> offVer(const MatrixDS<Element> &M, uint16_t k) {
    MatrixDS<Element> C = MatrixDS<Element>(k, M.nrows()), SVK;

    do {
        fill_matrix_randomly(C);
        SVK = C * M;
    } while(!SVK.has_full_row_rank());

    return std::make_tuple(C, SVK);
}

#endif
