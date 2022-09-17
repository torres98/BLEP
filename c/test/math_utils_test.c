#ifndef ELEM_TYPE
    #define ELEM_TYPE int
#endif

#include <assert.h>

#include "../include/math_utils.h"

#include "int_matrix_samples.h"

#include <stdio.h>

int main() {

    //MatrixDS

    //Standard constructor
    MatrixDS* A = CreateMatrix(80, 80, true);
    assert(!any(A));

    //Copy constructor
    MatrixDS *A_copy = CopyMatrix(A);
    assert(equal(A, A_copy));

    //Getter and setter
    #if ELEM_BIT_SIZE == 4
        for (int i = 0; i < 80; i++) {
            for (int j = 0; j < 80; j++) {
                set(A, i, j, (i + j) & 0x7);
                assert(get(A, i, j) == ((i + j) & 0x7));
            }
        }
    #else
        
    #endif

    //Product operator
    #if ELEM_BIT_SIZE == 4

    #else
        MatrixDS *A1 = CreateMatrixFromArray((ELEM_TYPE*) M1, 2, 3, false), *A2 = CreateMatrixFromArray((ELEM_TYPE*) M2, 3, 4, false), *O = CreateMatrixFromArray((ELEM_TYPE*) M1_times_M2, 2, 4, false);
        assert(equal(dot_product(A1, A2), O));

        destroy_matrix(A1);
        destroy_matrix(A2);
        destroy_matrix(O);
    #endif

    destroy_matrix(A);
    destroy_matrix(A_copy);

    return 0;
}