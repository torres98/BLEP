#include <assert.h>

#include "../include/math_utils.h"
#include "../include/random_utils.h"

#include "matrix_samples.h"


int main() {

    //Check that the constructor fills the matrix with the default values
    MatrixDS<int> M_int = MatrixDS<int>(80, 80, true);
    int default_value = int();

    for (unsigned int i = 0; i < M_int.nrows(); i++)
        for (unsigned int j = 0; j < M_int.ncolumns(); j++)
            assert(M_int(i, j) == default_value);

    //Constructor from 2D array

    //Copy constructor
    MatrixDS<int> M_int_copy = M_int;

    assert(&M_int_copy != &M_int);
    assert(M_int_copy == M_int);

    //Product operator
    MatrixDS<int> A = MatrixDS<int>((int*) M1, 2, 3), B = MatrixDS<int>((int*) M2, 3, 4);
    assert(A*B == MatrixDS<int>((int*) M1_times_M2, 2, 4));

    //Assignment operator
    VectorDS<int> v = VectorDS<int>(10);

    try {
        v.set(10, 10);
    } catch (std::invalid_argument& e) {
        std::cout << "Exception correctly caught." << std::endl;
    }

    //full rank test
    assert(!MatrixDS<int>((int*) A1, 2, 3).has_full_rank());
    assert(!MatrixDS<int>((int*) A2, 3, 3).has_full_rank());
    assert(!MatrixDS<int>((int*) A3, 3, 3).has_full_rank());
    assert(MatrixDS<int>((int*) A4, 3, 3).has_full_rank());

    return 0;
}
