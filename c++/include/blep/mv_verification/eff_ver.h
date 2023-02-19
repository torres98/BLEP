#ifndef EFF_VER_H
#define EFF_VER_H

#include <tuple>
#include <stdexcept>

#include "blep/math/matrix.h"
#include "blep/math/vector.h"


/**
 * Randomly generate a valid short verification key with the specified number of rows.
 * 
 * @param VK MatrixDS object representing a verification key.
 * @param k Number of rows of the short verification key.
 * 
 * @return Tuple containing a randomly generated linear transformation and the associated
 *         short verification key.
 * 
 * @throw Throws a std::invalid_argument exception when the parameter k is greater than
 *        the number of rows of the verification key.
 */
template <typename Element>
std::tuple<MatrixDS<Element>, MatrixDS<Element>> offVer(const MatrixDS<Element> &VK, uint16_t k) {
    uint16_t vk_nrows = VK.nrows();

    if (k > vk_nrows) {
        std::ostringstream error_log;
        error_log << "Security parameter k (" << k << ") can't be bigger than the number of rows of the verification key (" << vk_nrows << ")";
        throw std::invalid_argument(error_log.str());
    }

    MatrixDS<Element> C = MatrixDS<Element>(k, vk_nrows), SVK;

    do {
        fill_matrix_randomly(C);
        SVK = C * VK;
    } while(!SVK.has_full_row_rank());

    return std::make_tuple(C, SVK);
}

/**
 * Randomly generate a valid extended short verification key with the specified number of rows.
 * 
 * @param VK MatrixDS object representing an extended verification key.
 * @param k Number of rows of the extended short verification key.
 * 
 * @return Randomly generated short verification key.
 * 
 * @throw Throws a std::invalid_argument exception when the parameter k is greater than
 *        the number of rows of the extended verification key.
 */
template <typename Element>
MatrixDS<Element> offVer_ext(const MatrixDS<Element> &VK_ext, uint16_t k) {
    uint16_t vk_nrows = VK_ext.nrows();

    if (k > vk_nrows) {
        std::ostringstream error_log;
        error_log << "Security parameter k (" << k << ") can't be bigger than the number of rows of the extended verification key (" << vk_nrows << ")";
        throw std::invalid_argument(error_log.str());
    }

    MatrixDS<Element> C = MatrixDS<Element>(k, vk_nrows), SVK_ext;

    do {
        fill_matrix_randomly(C);
        SVK_ext = C * VK_ext;
    } while(!SVK_ext.has_full_row_rank());

    return SVK_ext;
}

#endif
