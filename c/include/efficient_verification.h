#ifndef EFFICIENT_VERIFICATION_H
#define EFFICIENT_VERIFICATION_H

#include <stdint.h>

#include "math_utils.h"


MatrixDS* generate_random_linear_transformation(uint16_t rows, uint16_t columns);
MatrixDS* offVer(const MatrixDS* M, MatrixDS* C, uint16_t k);

#endif
