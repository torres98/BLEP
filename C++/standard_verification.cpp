#include "matrix.cpp"

#ifndef _STANDARD_VERIFICATION
#define _STANDARD_VERIFICATION

template <typename Element>
bool verify_signature(Matrix<Element> &M, Matrix<Element> &v) {
    return (M * v).is_zero();
}

#endif