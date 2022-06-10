#ifndef RANDOM_UTILS_H
#define RANDOM_UTILS_H

#include <algorithm>

#include <zephyr/random/rand32.h>

#include "matrix.h"


template <typename Element>
void fill_matrix_randomly(Matrix<Element> &M, uint8_t min = 0, uint8_t max = 255) {

    for (unsigned int i = 0; i < M.nrows(); i++)
        for (unsigned int j = 0; j < M.ncolumns(); j++)
            M[i][j] = Element(sys_rand32_get());

}

/*template <typename Element>
void shuffle_array(Element* arr, unsigned int n) {
    shuffle(arr, arr + n, rng);
}*/

#endif
