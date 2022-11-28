#ifndef STANDARD_VERIFICATION_H
#define STANDARD_VERIFICATION_H

#include "math_utils.h"


/**
 * Verify the given signature vector.
 * 
 * @param VK MatrixDS object representing a verification key.
 * @param s VectorDS object representing a signature.
 * @param u VectorDS object containing message-related informations.
 * 
 * @return true if the verification succeeds (i.e. when VK * s == u), false otherwise.
 * 
 * @throw Throws std::invalid_argument if one of the following condition is met:
 *            - the number of columns of the verification key is different from the size
 *              of the signature vector
 *            - the number of rows of the verification key is different from the size
 *              of the result vector.
 */
template <typename Element>
bool verify_signature(const MatrixDS<Element> &VK, const VectorDS<Element> &s, const VectorDS<Element> &u) {
    uint16_t vk_nrows = VK.nrows();

    if (VK.ncolumns() != s.size()) {
        std::ostringstream error_log;
        error_log << "Incompatible sizes for signature verification (verification key of size " << vk_nrows << "x" << VK.ncolumns() << " and signature of size " << s.size() << ")";
        throw std::invalid_argument(error_log.str());
    } else if (vk_nrows != u.size()) {
        std::ostringstream error_log;
        error_log << "Incompatible sizes for signature verification (verification key of size " << vk_nrows << "x" << VK.ncolumns() << " and resulting vector of size " << u.size() << ")";
        throw std::invalid_argument(error_log.str());
    }

    bool verification_result = true;

    for (uint16_t i = 0; i < vk_nrows; i++)
        if (VK.row_vector_product(i, s) != u(i))
            verification_result = false;

    return verification_result;
}

/**
 * Verify the given extended signature vector.
 * 
 * @param VK_ext MatrixDS object representing an extended verification key.
 * @param s_ext VectorDS object representing an extended signature.
 * 
 * @return true if the verification succeeds (i.e. when VK_ext * s_ext == 0), false otherwise.
 * 
 * @throw Throws std::invalid_argument if the number of columns of the extended verification
 *        key is different from the size of the extended signature vector.
 */
template <typename Element>
bool verify_signature(const MatrixDS<Element> &VK_ext, const VectorDS<Element> &s_ext) {
    if (VK_ext.ncolumns() != s_ext.size()) {
        std::ostringstream error_log;
        error_log << "Incompatible sizes for signature verification (extended verification key of size " << VK_ext.nrows() << "x" << VK_ext.ncolumns() << " and extended signature of size " << s_ext.size() << ")";
        throw std::invalid_argument(error_log.str());
    }

    bool verification_result = true;

    for (uint16_t i = 0; i < VK_ext.nrows(); i++)
        if (VK_ext.row_vector_product(i, s_ext))
            verification_result = false;

    return verification_result;
}

#endif
