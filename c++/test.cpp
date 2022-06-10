#include <assert.h>

#include "include/matrix.h"

int main() {

    int v0[2][3] = {
        {1, 2, 3},
        {3, 6, 9}
    };

    int v1[3][3] = {
        {1, 2, 3},
        {0, 2, 2},
        {1, 4, 5}
    };

    int v2[3][3] = {
        {1, 2, 3},
        {0, 2, 2},
        {1, -2, -1}
    };

    int v3[3][3] = {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
    };

    Matrix<int> M0 = Matrix<int>(v0);
    assert(!M0.has_full_rank());

    Matrix<int> M1 = Matrix<int>(v1);
    assert(!M1.has_full_rank());

    Matrix<int> M2 = Matrix<int>(v2);
    assert(!M2.has_full_rank());

    Matrix<int> M3 = Matrix<int>(v3);
    assert(M3.has_full_rank());

    return 0;
}