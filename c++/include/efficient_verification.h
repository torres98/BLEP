#ifndef EFFICIENT_VERIFICATION_H
#define EFFICIENT_VERIFICATION_H

#include <tuple>

#include "math_utils.h"

/**
 * Randomly generate a linear transformation ... short verification key.
 * 
 * @param VK MatrixDS object representing a verification key.
 * @param k Number of rows of the SVK.
 * 
 * @return Tuple containing a randomly generated linear transformation and the associated
 *         short verification key.
 * 
 * @throw Throws a std::invalid_argument exception when the security parameter is greater
 *        than the number of rows of the verification key.
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

#endif
