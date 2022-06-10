#ifndef EFFICIENT_VERIFICATION_H
#define EFFICIENT_VERIFICATION_H

#include "matrix.h"
#include "vector.h"

/*
template <typename Element>
Matrix<Element> offVer(Matrix<Element> &M, unsigned int k, bool std=true) {

    Matrix<Element> C = Matrix<Element>(k, M.nrows()); 
    fill_matrix_randomly(C); 

    //while not np.linalg.matrix_rank(C) == k:
    //    C = np.random.randint(0, q, (k, n))

    if (std)
        return C * M;
    
    return C * M || C;
}
*/

template <typename Element>
Matrix<Element> generate_random_linear_transformation(unsigned int nrows, unsigned int ncolumns) {
    Matrix<Element> C = Matrix<Element>(nrows, ncolumns); 
    
    do {
        fill_matrix_randomly(C); 
    } while(!C.has_full_rank());

    return C;
}

template <typename Element>
Matrix<Element> offVer(Matrix<Element> &M, unsigned int k) {

    Matrix<Element> C = generate_random_linear_transformation<Element>(k, M.nrows());

    return C * M;
}

#endif
