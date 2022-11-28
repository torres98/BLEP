#ifndef PROGRESSIVE_VERIFICATION_H
#define PROGRESSIVE_VERIFICATION_H

#include <sstream>

#include "math_utils.h"
#include "random_utils.h"


/**
 * Progressively verify the given signature.
 * 
 * @param VK MatrixDS object representing a verification key.
 * @param s VectorDS object representing a signature.
 * @param u VectorDS object containing message-related informations.
 * @param t Number of verification steps to be computed.
 * @param rand If true randomizes the order in which the verification key rows are evaluated
 *             (by default true).
 * 
 * @return true if all the computed verification steps are successful, false otherwise.
 * 
 * @throw Throws a std::invalid_argument exception when one of the following conditions is met:
 *            - the number of columns of the verification key is different from the signature size
 *            - the number of rows of the verification key is different from the size of the result
 *              vector
 *            - the number of verification steps is greater than the number of rows of the verification
 *              key
 */
template <typename Element>
bool progVer(const MatrixDS<Element> &VK, const VectorDS<Element> &s, const VectorDS<Element> &u, uint16_t t, bool rand=true) {
    uint16_t vk_nrows = VK.nrows(), sig_size = s.nrows();

    if (VK.ncolumns() != sig_size) {
        std::ostringstream error_log;
        error_log << "Incompatible sizes for progressive signature verification (verification key of size " << vk_nrows << "x" << VK.ncolumns() << " and signature of size " << sig_size << ")";
        throw std::invalid_argument(error_log.str());
    } else if (vk_nrows != u.nrows()) {
        std::ostringstream error_log;
        error_log << "Incompatible sizes for signature verification (verification key of size " << vk_nrows << "x" << VK.ncolumns() << " and resulting vector of size " << u.nrows() << ")";
        throw std::invalid_argument(error_log.str());
    } else if (t > vk_nrows) {
        std::stringstream error_log;
        error_log << "Number of steps t (" << t << ") can't be bigger than the number of rows of the verification key (" << vk_nrows << ")";
        throw std::invalid_argument(error_log.str());
    }

    if (t == 0)
        return false;

    bool verification_result = true;

    if (rand) {
        uint16_t* row_indices = new uint16_t[vk_nrows];

        for (uint16_t i = 0; i < vk_nrows; i++)
            row_indices[i] = i;
        
        shuffle_array(row_indices, vk_nrows);

        for (uint16_t i = 0; i < t; i++)
            if (VK.row_vector_product(row_indices[i], s) != u(row_indices[i]))
                verification_result = false;

        delete[] row_indices;
    } else {
        for (uint16_t i = 0; i < t; i++)
            if (VK.row_vector_product(i, s) != u(i))
                verification_result = false;
    }

    return verification_result;
}

/**
 * Progressively verify the given extended signature.
 * 
 * @param VK_ext MatrixDS object representing an extended verification key.
 * @param s_ext VectorDS object representing an extended signature.
 * @param t Number of verification steps to be computed.
 * @param rand If true randomizes the order in which the extended verification key rows are
 *             evaluated (by default true)
 * 
 * @return true if all the computed verification steps are successful, false otherwise.
 * 
 * @throw Throws a std::invalid_argument exception when one of the following conditions is met:
 *            - the number of columns of the extended verification key is different from the
 *              extended signature size
 *            - the number of verification steps is greater than the number of rows of the
 *              extended verification key
 */
template <typename Element>
bool progVer(const MatrixDS<Element> &VK_ext, const VectorDS<Element> &s_ext, uint16_t t, bool rand=true) {
    uint16_t vk_nrows = VK_ext.nrows(), sig_size = s_ext.nrows();

    if (VK_ext.ncolumns() != sig_size) {
        std::ostringstream error_log;
        error_log << "Incompatible sizes for progressive signature verification (extended verification key of size " << vk_nrows << "x" << VK_ext.ncolumns() << " and extended signature of size " << sig_size << ")";
        throw std::invalid_argument(error_log.str());
    } else if (t > vk_nrows) {
        std::stringstream error_log;
        error_log << "The number of steps (" << t << ") can't be bigger than the number of rows of the extended verification key (" << vk_nrows << ")";
        throw std::invalid_argument(error_log.str());
    }

    if (t == 0)
        return false;

    bool verification_result = true;

    if (rand) {
        uint16_t* row_indices = new uint16_t[vk_nrows];

        for (uint16_t i = 0; i < vk_nrows; i++)
            row_indices[i] = i;
    
        shuffle_array(row_indices, VK_ext.nrows());

        for (uint16_t i = 0; i < t; i++)
            if (VK_ext.row_vector_product(row_indices[i], s_ext))
                verification_result = false;

        delete[] row_indices;
    } else {
        for (uint16_t i = 0; i < t; i++)
            if (VK_ext.row_vector_product(i, s_ext))
                verification_result = false;
    }

    return verification_result;
}

#endif
