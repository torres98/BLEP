#include <cassert>

#include "blep/math/matrix.h"
#include "blep/utils/rand.h"
#include "blep/math/vector.h"

#include "matrix_samples.h"


int main() {
    //MatrixDS

    //Main constructor
    MatrixDS<int> M_int = MatrixDS<int>(80, 80, true);
    int default_value = int();

    for (unsigned int i = 0; i < M_int.nrows(); i++)
        for (unsigned int j = 0; j < M_int.ncolumns(); j++)
            assert(M_int(i, j) == default_value);

    //Getter
    try {
        M_int(M_int.nrows(), 10);
    } catch (std::invalid_argument& e) {}

    try {
        M_int(10, M_int.ncolumns());
    } catch (std::invalid_argument& e) {}

    //Copy constructor
    MatrixDS<int> M_int_copy = M_int;

    assert(&M_int_copy != &M_int);
    assert(M_int_copy == M_int);

    //Sum operator
    MatrixDS<int> S1_mat = MatrixDS<int>((int*) S1, 3, 5), S2_mat = MatrixDS<int>((int*) S2, 3, 5);
    assert(S1_mat + S2_mat == MatrixDS<int>((int*) S1_plus_S2, 3, 5));

    //Sub operator
    assert(S1_mat - S2_mat == MatrixDS<int>((int*) S1_minus_S2, 3, 5));

    //Product operator
    MatrixDS<int> A = MatrixDS<int>((int*) M1, 2, 3), B = MatrixDS<int>((int*) M2, 3, 4);
    assert(A*B == MatrixDS<int>((int*) M1_times_M2, 2, 4));

    //Full row rank test
    assert(!MatrixDS<int>((int*) A1, 2, 3).has_full_row_rank());
    assert(!MatrixDS<int>((int*) A2, 3, 3).has_full_row_rank());
    assert(!MatrixDS<int>((int*) A3, 3, 3).has_full_row_rank());
    assert(MatrixDS<int>((int*) A4, 3, 3).has_full_row_rank());


    //VectorDS

    //Assignment operator
    VectorDS<int> v = VectorDS<int>(10);

    try {
        v.set(10, 10);
    } catch (std::invalid_argument& e) {}

    return 0;
}
