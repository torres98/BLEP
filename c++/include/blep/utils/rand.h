#ifndef RAND_H
#define RAND_H

#include <algorithm>
#include <random>

#include "blep/math/matrix.h"


extern std::mt19937 rng;

void fill_buffer_randomly(unsigned char* buffer, size_t buf_size);

template <typename Element>
void fill_matrix_randomly(MatrixDS<Element> &A, uint8_t min, uint8_t max, uint32_t seed) {
    if (seed != 0)  
        rng.seed(seed);

    std::uniform_int_distribution<uint8_t> element_dist(min, max);
    
    for (uint32_t i = 0; i < A.size(); i++)
        A.M[i] = Element(element_dist(rng));
}

template <typename Element>
void shuffle_array(Element* arr, unsigned int n) {
    std::shuffle(arr, arr + n, rng);
}

#endif
