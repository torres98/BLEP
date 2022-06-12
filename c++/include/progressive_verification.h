#ifndef PROGRESSIVE_VERIFICATION_H
#define PROGRESSIVE_VERIFICATION_H

#include <sstream>

#include "matrix.h"
#include "random_utils.h"
#include "vector.h"

template <typename Element>
bool progVer(const Matrix<Element> &M, const Vector<Element> &v, unsigned int t, bool rand=true) {

    if (M.ncolumns() != v.nrows()) {
        std::ostringstream error_log;
        error_log << "Incompatible sizes for progressive signature verification (matrix of size " << M.nrows() << "x" << M.ncolumns() << " and vector of size " << (v.nrows() == 1 ? v.ncolumns(): v.nrows()) << ")";
        throw std::invalid_argument(error_log.str());
    }

    if (t > M.nrows()) {
        std::stringstream error_log;
        error_log << "The number of steps (" << t << ") can't be bigger than the number of rows of the matrix (" << M.nrows() << ")";
        throw std::invalid_argument(error_log.str());
    }

    if (t == 0)
        return false;

    //seems that this amount of memory is necessary...
    unsigned int* row_indices = new unsigned int[M.nrows()];

    for (unsigned int i = 0; i < M.nrows(); i++)
        row_indices[i] = i;
    
    if (rand)
        shuffle_array(row_indices, M.nrows());

    Element cell;
    bool verification_result = true;

    for (unsigned int i = 0; i < t; i++) {
        cell = Element();

        for (unsigned int j = 0; j < v.nrows(); j++)
            cell += M(row_indices[i], j) * v(j);

        if (cell)
            verification_result = false;
    }

    delete[] row_indices;

    return verification_result;
}

template <typename Element>
bool progVer_lazy(const Matrix<Element> &M, const Vector<Element> &v, unsigned int t, bool rand=true) {

    if (M.ncolumns() != v.nrows()) {
        std::ostringstream error_log;
        error_log << "Incompatible sizes for progressive signature verification (matrix of size " << M.nrows() << "x" << M.ncolumns() << " and vector of size " << (v.nrows() == 1 ? v.ncolumns(): v.nrows()) << ")";
        throw std::invalid_argument(error_log.str());
    }

    if (t > M.nrows()) {
        std::stringstream error_log;
        error_log << "The number of steps (" << t << ") can't be bigger than the number of rows of the matrix (" << M.nrows() << ")";
        throw std::invalid_argument(error_log.str());
    }

    if (t == 0)
        return false;

    unsigned int* row_indices = new unsigned int[M.nrows()];

    for (unsigned int i = 0; i < M.nrows(); i++)
        row_indices[i] = i;
    
    if (rand)
        shuffle_array(row_indices, M.nrows());

    Element cell;

    for (unsigned int i = 0; i < t; i++) {
        cell = Element();

        for (unsigned int j = 0; j < v.nrows(); j++)
            cell += M(row_indices[i], j) * v(j);

        if (cell) {
            delete[] row_indices;

            return false;
        }
    }

    delete[] row_indices;

    return true;
}

template <typename Element>
bool progVer(const Matrix<Element> &PK, const Vector<Element> &s, const Vector<Element> &u, unsigned int t, bool rand=true) {

    if (PK.ncolumns() != s.nrows()) {
        std::ostringstream error_log;
        error_log << "Incompatible sizes for progressive signature verification (public key of size " << PK.nrows() << "x" << PK.ncolumns() << " and signature of size " << (s.nrows() == 1 ? s.ncolumns(): s.nrows()) << ")";
        throw std::invalid_argument(error_log.str());
    }

    if (PK.nrows() != u.nrows()) {
        std::ostringstream error_log;
        error_log << "Incompatible sizes for signature verification (public key of size " << PK.nrows() << "x" << PK.ncolumns() << " and resulting vector of size " << (u.nrows() == 1 ? u.ncolumns(): u.nrows()) << ")";
        throw std::invalid_argument(error_log.str());
    }

    if (t > PK.nrows()) {
        std::stringstream error_log;
        error_log << "Number of steps t (" << t << ") can't be bigger than the number of rows of the public key (" << PK.nrows() << ")";
        throw std::invalid_argument(error_log.str());
    }

    if (t == 0)
        return false;

    unsigned int* row_indices = new unsigned int[PK.nrows()];

    for (unsigned int i = 0; i < PK.nrows(); i++)
        row_indices[i] = i;
    
    if (rand)
        shuffle_array(row_indices, PK.nrows());

    Element cell;
    bool verification_result = true;

    for (unsigned int i = 0; i < t; i++) {
        cell = Element();

        for (unsigned int j = 0; j < s.nrows(); j++)
            cell += PK(row_indices[i], j) * s(j);

        if (cell != u(row_indices[i]))
            verification_result = false;
    }

    delete[] row_indices;

    return verification_result;
}

template <typename Element>
bool progVer_lazy(const Matrix<Element> &PK, const Vector<Element> &s, const Vector<Element> &u, unsigned int t, bool rand=true) {

    if (PK.ncolumns() != s.nrows()) {
        std::ostringstream error_log;
        error_log << "Incompatible sizes for progressive signature verification (public key of size " << PK.nrows() << "x" << PK.ncolumns() << " and signature of size " << (s.nrows() == 1 ? s.ncolumns(): s.nrows()) << ")";
        throw std::invalid_argument(error_log.str());
    }

    if (PK.nrows() != u.nrows()) {
        std::ostringstream error_log;
        error_log << "Incompatible sizes for signature verification (public key of size " << PK.nrows() << "x" << PK.ncolumns() << " and resulting vector of size " << (u.nrows() == 1 ? u.ncolumns(): u.nrows()) << ")";
        throw std::invalid_argument(error_log.str());
    }

    if (t > PK.nrows()) {
        std::stringstream error_log;
        error_log << "Number of steps t (" << t << ") can't be bigger than the number of rows of the public key (" << PK.nrows() << ")";
        throw std::invalid_argument(error_log.str());
    }

    if (t == 0)
        return false;

    unsigned int* row_indices = new unsigned int[PK.nrows()];

    for (unsigned int i = 0; i < PK.nrows(); i++)
        row_indices[i] = i;
    
    if (rand)
        shuffle_array(row_indices, PK.nrows());

    Element cell;

    for (unsigned int i = 0; i < t; i++) {
        cell = Element();

        for (unsigned int j = 0; j < s.nrows(); j++)
            cell += PK(row_indices[i], j) * s(j);

        if (cell != u(row_indices[i])) {
            delete[] row_indices;
            
            return false;
        }
    }

    delete[] row_indices;

    return true;
}

#endif
