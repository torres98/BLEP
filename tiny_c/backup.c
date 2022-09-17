#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/math_utils.h"
#include "../include/memory_utils.h"

#ifndef ELEM_TYPE
    #define ELEM_TYPE int
#endif

#if ELEM_TYPE == int || ELEM_TYPE == long || ELEM_TYPE == float || ELEM_TYPE == double 
    #define add(a, b) (a + b)
    #define mul(a, b) (a * b)
#endif

#ifdef ELEM_BIT_SIZE
    #define MATRIX_BYTE_SIZE(O) ceil((double) (O -> rows * O -> columns * ELEM_BIT_SIZE) / 8)
    //#define VECTOR_BYTE_SIZE(O) ceil((double) (O -> size * ELEM_BIT_SIZE) / 8)
#else
    #define MATRIX_BYTE_SIZE(O) O -> rows * O -> columns * sizeof(ELEM_TYPE)
    //#define VECTOR_BYTE_SIZE(O) O -> size * sizeof(ELEM_TYPE)
#endif

struct MatrixDS {
    uint16_t rows, columns;
    ELEM_TYPE *M;

    bool is_memory_static;
};

//CONSTRUCTORS
MatrixDS* CreateMatrix(uint16_t rows, uint16_t columns, bool init) {
    MatrixDS* O = (MatrixDS*) allocate_memory(sizeof(MatrixDS));

    O -> rows = rows;
    O -> columns = columns;
    O -> M = allocate_memory(MATRIX_BYTE_SIZE(O));

    if (init)
        memset(O -> M, 0, MATRIX_BYTE_SIZE(O));

    O -> is_memory_static = false;

    return O;
}

MatrixDS* CreateMatrixFromArray(ELEM_TYPE *M, uint16_t rows, uint16_t columns, bool copy) {
    MatrixDS* O = (MatrixDS*) allocate_memory(sizeof(MatrixDS));

    O -> rows = rows;
    O -> columns = columns;

    if (copy) {
        O -> M = allocate_memory(MATRIX_BYTE_SIZE(O));
        memcpy(O -> M, M, MATRIX_BYTE_SIZE(O));
    } else {
        O -> M = M;
    }

    O -> is_memory_static = !copy;

    return O;
}

MatrixDS* CreateMatrixFrom2DArray(uint16_t rows, uint16_t columns, ELEM_TYPE M[rows][columns]) {
    MatrixDS* O = (MatrixDS*) allocate_memory(sizeof(MatrixDS));

    O -> rows = rows;
    O -> columns = columns;
    O -> M = allocate_memory(MATRIX_BYTE_SIZE(O));
    memcpy(O -> M, M, MATRIX_BYTE_SIZE(O));

    O -> is_memory_static = false;

    return O;
}

MatrixDS* CopyMatrix(MatrixDS* A) {
    MatrixDS* O = (MatrixDS*) allocate_memory(sizeof(MatrixDS));

    O -> rows = A -> rows;
    O -> columns = A -> columns;
    O -> M = allocate_memory(MATRIX_BYTE_SIZE(O));

    O -> is_memory_static = false;

    return O;
}

uint16_t nrows(MatrixDS* A) {
    return A -> rows;
}

uint16_t ncolumns(MatrixDS* A) {
    return A -> columns;
}

uint16_t size(MatrixDS* A) {
    return A -> rows * A -> columns;
}

//get
ELEM_TYPE get(MatrixDS* A, uint16_t i, uint16_t j) {
    if (i > A -> rows) {
        fprintf(stderr, "Row index %u out of bounds for matrix of size %ux%u\n", i, A -> rows, A -> columns);
        exit(-1);
    }

    if (j > A -> columns) {
        fprintf(stderr, "Column index %u out of bounds for matrix of size %ux%u\n", j, A -> rows, A -> columns);
        exit(-1);
    }

    uint32_t offset = i * A -> columns + j;

    #if ELEM_BIT_SIZE == 4
        if ((offset & 0x1) == 0)
            return A -> M[offset >> 1] >> 4; //return high nibble
        else
            return A -> M[offset >> 1] & 0xf; //return low nibble
    #else
        return A -> M[offset];
    #endif
}

//set
void set(MatrixDS* A, uint16_t i, uint16_t j, ELEM_TYPE v) {
    if (i > A -> rows) {
        fprintf(stderr, "Row index %u out of bounds for matrix of size %ux%u", i, A -> rows, A -> columns);
        exit(-1);
    }

    if (j > A -> columns) {
        fprintf(stderr, "Column index %u out of bounds for matrix of size %ux%u", j, A -> rows, A -> columns);
        exit(-1);
    }

    uint32_t offset = i * A -> columns + j;

    #if ELEM_BIT_SIZE == 4
        if ((offset & 0x1) == 0)
            A -> M[offset >> 1] = (v << 4) | (A -> M[offset >> 1] & 0xf); //set just the high nibble
        else
            A -> M[offset >> 1] = (A -> M[offset >> 1] & 0xf0) | v; //set just the low nibble
    #else
        A -> M[offset] = v;
    #endif
}

//Product
MatrixDS* dot_product(MatrixDS *A, MatrixDS *B) {
    if (A -> columns != B -> rows) {
        fprintf(stderr, "Incompatible sizes for product between matrices of size %ux%u and %ux%u", A -> rows, A -> columns, B -> rows, B -> columns);
        exit(-1);
    }

    uint16_t n = A -> rows, m = B -> columns, p = A -> columns;
    MatrixDS *O = CreateMatrix(n, m, true);
    
    #if ELEM_BIT_SIZE == 4
        for (uint16_t i = 0; i < n; i++) //fix
            for (uint16_t j = 0; j < m; j++) //fix
                for (uint16_t h = 0; h < p; h++) {//fix
                    set(O, i, j, add(get(O, i, j), mul(get(A, i, h), get(B, h, j))));
                }

    #else
        for (uint16_t i = 0; i < n; i++)
            for (uint16_t j = 0; j < m; j++)
                for (uint16_t h = 0; h < p; h++)
                    O -> M[i*m + j] = add(O -> M[i*m + j], mul((A -> M)[i*p + h], B -> M[h*m + j]));
    #endif

    return O;
}

bool equal(MatrixDS* A, MatrixDS* B) {
    if (A -> rows != B -> rows || A -> columns != B -> columns)
        return false;
    
    return memcmp(A -> M, B -> M, MATRIX_BYTE_SIZE(A)) == 0;
}

bool any(MatrixDS* A) {
    for (uint16_t i = 0; i < A -> rows; i++)
        for (uint16_t j = 0; j < A -> columns; j++)
            if (true)
                return false;

    return true;
}

void print_matrix(MatrixDS* A) {
    for (uint16_t i = 0; i < A -> rows; i++) {
        for (uint16_t j = 0; j < A -> columns; j++) 
            printf("%d ", get(A, i, j));
        printf("\n");
    }
}

//DESTRUCTOR
void destroy(MatrixDS* A) {

    if (!A -> is_memory_static)
        free_memory(A -> M);
    
    free(A);
}

/*
struct VectorDS {
    uint16_t size;
    ELEM_TYPE *V;

    bool is_memory_static;
};

VectorDS* CreateVector(uint16_t size, bool init) {
    VectorDS* O = (VectorDS*) allocate_memory(sizeof(VectorDS));

    O -> size = size;
    O -> V = allocate_memory(VECTOR_BYTE_SIZE(O));

    if (init)
        memset(O -> V, 0, VECTOR_BYTE_SIZE(O));

    O -> is_memory_static = false;

    return O;
}
*/