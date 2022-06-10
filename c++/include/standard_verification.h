#ifndef STANDARD_VERIFICATION_H
#define STANDARD_VERIFICATION_H

#include "matrix.h"
#include "vector.h"

template <typename Element>
bool verify_signature(const Matrix<Element> &M, const Vector<Element> &v) {

    if (M.ncolumns() != v.nrows()) {
        std::ostringstream error_log;
        error_log << "Incompatible sizes for signature verification (public key of size " << M.nrows() << "x" << M.ncolumns() << " and signature of size " << (v.nrows() == 1 ? v.ncolumns(): v.nrows()) << ")";
        throw std::invalid_argument(error_log.str());
    }

    Element cell;
    bool verification_result = true;

    for (unsigned int i = 0; i < M.nrows(); i++) {
        cell = Element();

        for (unsigned int j = 0; j < v.nrows(); j++)
            cell += M[i][j] * v[j];

        if (cell)
            verification_result = false;

    }

    return verification_result;
}

template <typename Element>
bool verify_signature_lazy(const Matrix<Element> &M, const Vector<Element> &v) {

    if (M.ncolumns() != v.nrows()) {
        std::ostringstream error_log;
        error_log << "Incompatible sizes for signature verification (public key of size " << M.nrows() << "x" << M.ncolumns() << " and signature of size " << (v.nrows() == 1 ? v.ncolumns(): v.nrows()) << ")";
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

template <typename Element>
bool verify_signature(const Matrix<Element> &PK, const Vector<Element> &s, const Vector<Element> &u) {

    if (PK.ncolumns() != s.nrows()) {
        std::ostringstream error_log;
        error_log << "Incompatible sizes for signature verification (public key of size " << PK.nrows() << "x" << PK.ncolumns() << " and signature of size " << (s.nrows() == 1 ? s.ncolumns(): s.nrows()) << ")";
        throw std::invalid_argument(error_log.str());
    } 
    
    if (PK.nrows() != u.nrows()) {
        std::ostringstream error_log;
        error_log << "Incompatible sizes for signature verification (public key of size " << PK.nrows() << "x" << PK.ncolumns() << " and resulting vector of size " << (u.nrows() == 1 ? u.ncolumns(): u.nrows()) << ")";
        throw std::invalid_argument(error_log.str());
    }

    Element cell;
    bool verification_result = true;

    for (unsigned int i = 0; i < PK.nrows(); i++) {
        cell = Element();

        for (unsigned int j = 0; j < s.nrows(); j++)
            cell += PK[i][j] * s[j];

        if (cell != u[i])
            verification_result = false;

    }

    return verification_result;
}

template <typename Element>
bool verify_signature_lazy(const Matrix<Element> &PK, const Vector<Element> &s, const Vector<Element> &u) {

    if (PK.ncolumns() != s.nrows()) {
        std::ostringstream error_log;
        error_log << "Incompatible sizes for signature verification (public key of size " << PK.nrows() << "x" << PK.ncolumns() << " and signature of size " << (s.nrows() == 1 ? s.ncolumns(): s.nrows()) << ")";
        throw std::invalid_argument(error_log.str());
    } 
    
    if (PK.nrows() != u.nrows()) {
        std::ostringstream error_log;
        error_log << "Incompatible sizes for signature verification (public key of size " << PK.nrows() << "x" << PK.ncolumns() << " and resulting vector of size " << (u.nrows() == 1 ? u.ncolumns(): u.nrows()) << ")";
        throw std::invalid_argument(error_log.str());
    }

    Element cell;

    for (unsigned int i = 0; i < PK.nrows(); i++) {
        cell = Element();

        for (unsigned int j = 0; j < s.nrows(); j++)
            cell += PK[i][j] * s[j];

        if (cell != u[i])
            return false;

    }

    return true;
}

#endif
