#ifndef PROG_VER_H
#define PROG_VER_H

#include <stdbool.h>

#include "blep/math/matrix.h"


bool progVer(const MatrixDS* M, const MatrixDS* v, uint16_t t, bool rand);
bool progVer_with_result_vector(const MatrixDS* PK, const MatrixDS* s, const MatrixDS* u, uint16_t t, bool rand);

#endif
