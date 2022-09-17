#include "../include/efficient_verification.h"


MatrixDS* generate_random_linear_transformation(uint16_t rows, uint16_t columns) {
    MatrixDS* C = CreateMatrix(rows, columns, false); 
    
    do {
        fill_matrix_randomly(C); 
    } while(!has_full_rank(C));

    return C;
}

MatrixDS* offVer(const MatrixDS* A, uint16_t k) {
    MatrixDS* C = generate_random_linear_transformation(k, nrows(A));
    MatrixDS* SVK = dot_product(C, A);
    destroy_matrix(C);

    return SVK;
}
