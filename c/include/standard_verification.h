#ifndef STANDARD_VERIFICATION_H
#define STANDARD_VERIFICATION_H

#include "math_utils.h"


bool verify_signature(const MatrixDS* M, const MatrixDS* v);
bool verify_signature_with_result_vector(const MatrixDS* PK, const MatrixDS* s, const MatrixDS* u);

#endif
