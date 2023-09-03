#define ELEM_TYPE int
#include "blep/math/matrix.h"

#include "int_matrix_samples.h"

#include <stdio.h>

#define assert(e) if ((e) == 0) return 1

int main() {
    //Standard constructor
    MatrixDS* A = CreateMatrix(80, 80, true);

    //Copy constructor
    MatrixDS *A_copy = CopyMatrix(A);
    assert(are_matrices_equal(A, A_copy));

    //Getter and setter
    for (int i = 0; i < 80; i++) {
        for (int j = 0; j < 80; j++) {
            set(A, i, j, i + j);
            assert(get(A, i, j) == i + j);
        }
    }

    MatrixDS *A1 = CreateMatrixFromArray((ELEM_TYPE*) M1, 2, 3, false);
    MatrixDS *A2 = CreateMatrixFromArray((ELEM_TYPE*) M2, 3, 4, false);
    MatrixDS *O = CreateMatrixFromArray((ELEM_TYPE*) M1_times_M2, 2, 4, false);
    assert(are_matrices_equal(dot_product(A1, A2), O));

    destroy_matrix(A1);
    destroy_matrix(A2);
    destroy_matrix(O);
    destroy_matrix(A);
    destroy_matrix(A_copy);

    printf("Matrix test: PASSED\n");

    return 0;
}