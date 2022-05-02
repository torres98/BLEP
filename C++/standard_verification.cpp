#ifndef _STANDARD_VERIFICATION
#define _STANDARD_VERIFICATION

#include "matrix.cpp"

template <typename Element>
bool verify_signature(Matrix<Element> &M, Vector<Element> &v) {

    if (M.ncolumns() != v.nrows()) {
        std::ostringstream error_log;
        error_log << "Incompatible sizes for signature verification (matrix of size " << M.nrows() << "x" << M.ncolumns() << " and vector of size " << (v.nrows() == 1 ? v.ncolumns(): v.nrows()) << ")";
        throw std::invalid_argument(error_log.str());
    }

    Element cell;

    for (unsigned int i = 0; i < M.nrows(); i++) {
        cell = Element();

        for (unsigned int j = 0; j < v.nrows(); j++)
                cell += M[i][j] * v[j];

        if (cell)
            return false;

    }

    return true;
}

#endif