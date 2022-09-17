#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#ifndef ELEM_TYPE
    #error "Matrix element type not defined"
#endif

#include <stdbool.h>
#include <stdint.h>


//MATRIX
typedef struct MatrixDS MatrixDS;

//Constructor
MatrixDS* CreateMatrix(uint16_t rows, uint16_t columns, bool init);
MatrixDS* CreateMatrixFromArray(ELEM_TYPE* array, uint16_t rows, uint16_t columns, bool copy);
MatrixDS* CopyMatrix(const MatrixDS* A);

void fill_matrix_randomly(MatrixDS* A);

uint16_t nrows(const MatrixDS* A);
uint16_t ncolumns(const MatrixDS* A);
uint32_t size(const MatrixDS* A);

ELEM_TYPE get(const MatrixDS* A, uint16_t i, uint16_t j);
void set(MatrixDS* A, uint16_t i, uint16_t j, ELEM_TYPE v);

MatrixDS* dot_product(const MatrixDS* A, const MatrixDS* B);
ELEM_TYPE row_vector_product(const MatrixDS* A, const MatrixDS* v, uint16_t row_index);

bool are_matrices_equal(const MatrixDS* A, const MatrixDS* B);
bool has_full_rank(const MatrixDS* A);

void print_matrix(const MatrixDS* A);

//Destructor
void destroy_matrix(MatrixDS* A);

//VECTOR
//typedef struct VectorDS VectorDS;

#endif
