#ifndef STANDARD_VERIFICATION_H
#define STANDARD_VERIFICATION_H

#include "math_utils.h"


template <typename Element>
bool verify_signature(const MatrixDS<Element> &M, const VectorDS<Element> &v) {
    if (M.ncolumns() != v.nrows()) {
        std::ostringstream error_log;
        error_log << "Incompatible sizes for signature verification (public key of size " << M.nrows() << "x" << M.ncolumns() << " and signature of size " << (v.nrows() == 1 ? v.ncolumns(): v.nrows()) << ")";
        throw std::invalid_argument(error_log.str());
    }

    bool verification_result = true;

    for (uint16_t i = 0; i < M.nrows(); i++)
        if (M.row_vector_product(i, v))
            verification_result = false;

    return verification_result;
}


template <typename Element>
bool verify_signature(const MatrixDS<Element> &PK, const VectorDS<Element> &s, const VectorDS<Element> &u) {
    uint16_t pk_nrows = PK.nrows();

    if (PK.ncolumns() != s.nrows()) {
        std::ostringstream error_log;
        error_log << "Incompatible sizes for signature verification (public key of size " << pk_nrows << "x" << PK.ncolumns() << " and signature of size " << (s.nrows() == 1 ? s.ncolumns(): s.nrows()) << ")";
        throw std::invalid_argument(error_log.str());
    } else if (pk_nrows != u.nrows()) {
        std::ostringstream error_log;
        error_log << "Incompatible sizes for signature verification (public key of size " << pk_nrows << "x" << PK.ncolumns() << " and resulting vector of size " << (u.nrows() == 1 ? u.ncolumns(): u.nrows()) << ")";
        throw std::invalid_argument(error_log.str());
    }

    bool verification_result = true;

    for (uint16_t i = 0; i < pk_nrows; i++)
        if (PK.row_vector_product(i, s) != u(i))
            verification_result = false;

    return verification_result;
}

#endif
