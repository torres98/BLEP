#ifndef PROGRESSIVE_VERIFICATION_H
#define PROGRESSIVE_VERIFICATION_H

#include <sstream>

#include "math_utils.h"
#include "random_utils.h"


template <typename Element>
bool progVer(const MatrixDS<Element> &M, const VectorDS<Element> &v, uint16_t t, bool rand=true) {
    uint16_t pk_nrows = M.nrows(), sig_size = v.nrows();

    if (M.ncolumns() != sig_size) {
        std::ostringstream error_log;
        error_log << "Incompatible sizes for progressive signature verification (matrix of size " << pk_nrows << "x" << M.ncolumns() << " and vector of size " << sig_size << ")";
        throw std::invalid_argument(error_log.str());
    } else if (t > pk_nrows) {
        std::stringstream error_log;
        error_log << "The number of steps (" << t << ") can't be bigger than the number of rows of the matrix (" << pk_nrows << ")";
        throw std::invalid_argument(error_log.str());
    }

    if (t == 0)
        return false;

    bool verification_result = true;

    if (rand) {
        uint16_t* row_indices = new uint16_t[pk_nrows];

        for (uint16_t i = 0; i < pk_nrows; i++)
            row_indices[i] = i;
    
        shuffle_array(row_indices, M.nrows());

        for (uint16_t i = 0; i < t; i++)
            if (M.row_vector_product(row_indices[i], v))
                verification_result = false;

        delete[] row_indices;
    } else {
        for (uint16_t i = 0; i < t; i++)
            if (M.row_vector_product(i, v))
                verification_result = false;
    }

    return verification_result;
}

template <typename Element>
bool progVer(const MatrixDS<Element> &PK, const VectorDS<Element> &s, const VectorDS<Element> &u, uint16_t t, bool rand=true) {
    uint16_t pk_nrows = PK.nrows(), sig_size = s.nrows();

    if (PK.ncolumns() != sig_size) {
        std::ostringstream error_log;
        error_log << "Incompatible sizes for progressive signature verification (public key of size " << pk_nrows << "x" << PK.ncolumns() << " and signature of size " << sig_size << ")";
        throw std::invalid_argument(error_log.str());
    } else if (pk_nrows != u.nrows()) {
        std::ostringstream error_log;
        error_log << "Incompatible sizes for signature verification (public key of size " << pk_nrows << "x" << PK.ncolumns() << " and resulting vector of size " << u.nrows() << ")";
        throw std::invalid_argument(error_log.str());
    } else if (t > pk_nrows) {
        std::stringstream error_log;
        error_log << "Number of steps t (" << t << ") can't be bigger than the number of rows of the public key (" << pk_nrows << ")";
        throw std::invalid_argument(error_log.str());
    }

    if (t == 0)
        return false;

    bool verification_result = true;

    if (rand) {
        uint16_t* row_indices = new uint16_t[pk_nrows];

        for (uint16_t i = 0; i < pk_nrows; i++)
            row_indices[i] = i;
        
        shuffle_array(row_indices, pk_nrows);

        for (uint16_t i = 0; i < t; i++)
            if (PK.row_vector_product(row_indices[i], s) != u(row_indices[i]))
                verification_result = false;

        delete[] row_indices;
    } else {
        for (uint16_t i = 0; i < t; i++)
            if (PK.row_vector_product(i, s) != u(i))
                verification_result = false;
    }

    return verification_result;
}

#endif
