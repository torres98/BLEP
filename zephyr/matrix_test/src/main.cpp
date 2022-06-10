#include <assert.h>
#include <iostream>
#include <cstdint>

#include <zephyr/kernel.h>

#include "matrix_samples.h"
#include "tiny/include/matrix.h"
#include "tiny/include/vector.h"
#include "tiny/include/random_utils.h"


int main() {

    // timing variables
    uint32_t start_time, end_time;
    start_time = k_cycle_get_32();

    //Check that the constructor fills the matrix with the default values
    Matrix<int> M_int = Matrix<int>(70, 70, true);
    int default_value = int();

    for (unsigned int i = 0; i < M_int.nrows(); i++)
        for (unsigned int j = 0; j < M_int.ncolumns(); j++)
            assert(M_int[i][j] == default_value);

    //Constructor from 2D array

    //Copy constructor
    Matrix<int> M_int_copy = M_int;

    assert(&M_int_copy != &M_int);
    assert(M_int_copy == M_int);

    //Slicing constructor
    Matrix<int> M = Matrix<int>(10, 10);
    fill_matrix_randomly(M);

    unsigned int row_i = 1, row_f = 3, col_i = 2, col_f = 4;
    Matrix<int> M_slice = Matrix<int>(M, row_i, row_f, col_i, col_f);
    assert(M_slice.nrows() == row_f-row_i && M_slice.ncolumns() == col_f-col_i);
    
    for (unsigned int i = 0; i < M_slice.nrows(); i++)
        for (unsigned int j = 0; j < M_slice.ncolumns(); j++)
            assert(M_slice[i][j] == M[row_i+i][col_i+j]);

    std::cout << M << M_slice;

    //Product operator
    Matrix<int> A = Matrix<int>(M1), B = Matrix<int>(M2);
    assert(A*B == Matrix<int>(M1_times_M2));

    std::cout << A;
    
    //Assignment operator
    Vector<int> v = Vector<int>(10);

    try {
        v[10] = 10;
    } catch (std::invalid_argument& e) {
        std::cout << "Exception correctly caught." << std::endl;
    }
    
    std::cout << v;

    end_time = k_cycle_get_32() - start_time;
    std::cout << "Time required for matrix_test: " << k_cyc_to_us_floor64(end_time) << " microseconds" << std::endl;

    return 0;
}
