#ifndef RANDOM_UTILS_H
#define RANDOM_UTILS_H

#include <algorithm>
#include <random>

#include <zephyr/random/rand32.h>

#include "blep/math/matrix.h"

static std::mt19937 rng(sys_rand32_get());


template <typename Element>
void fill_matrix_randomly(MatrixDS<Element> &A, uint8_t min, uint8_t max) {
    std::uniform_int_distribution<uint8_t> element_dist(min, max);
    
    for (uint32_t i = 0; i < A.size(); i++)
        A.M[i] = Element(element_dist(rng));
}

template <typename Element>
void shuffle_array(Element* arr, unsigned int n) {
    shuffle(arr, arr + n, rng);
}

#endif
