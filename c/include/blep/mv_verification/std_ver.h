#ifndef STD_VER_H
#define STD_VER_H

#include <stdbool.h>

#include "blep/math/matrix.h"


bool verify_signature(const MatrixDS* M, const MatrixDS* v);
bool verify_signature_with_result_vector(const MatrixDS* PK, const MatrixDS* s, const MatrixDS* u);

#endif
