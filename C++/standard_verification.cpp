#ifndef _STANDARD_VERIFICATION
#define _STANDARD_VERIFICATION

#include "matrix.cpp"

template <typename Element>
bool verify_signature(Matrix<Element> &M, Vector<Element> &v) {
    return (M * v).is_zero();
}

#endif