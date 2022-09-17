#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/math_utils.h"

#include "../include/memory_utils.h"
#include "../include/random_utils.h"

//for now only 4 bits is accepted
#if ELEM_BIT_SIZE == 4
    typedef struct {
        ELEM_TYPE high_nibble:4, low_nibble:4;
    } NibbleElem;

    struct MatrixDS {
        NibbleElem *M;
        uint16_t rows, columns;

        bool is_memory_static;
    };

    #define MATRIX_BYTE_SIZE(O) ((O -> rows * O -> columns) >> 1) + (((O -> rows * O -> columns) & 0x1) == 1 ? 1: 0)
    //#define VECTOR_BYTE_SIZE(O) ceil((double) (O -> size * ELEM_BIT_SIZE) / 8)

    #define GET_BY_OFFSET(A, offset) (offset & 0x1) == 0 ? (A -> M[offset >> 1]).high_nibble: (A -> M[offset >> 1]).low_nibble

#else
    struct MatrixDS {
        ELEM_TYPE* M;
        uint16_t rows, columns;

        bool is_memory_static;
    };

    #define MATRIX_BYTE_SIZE(O) O -> rows * O -> columns * sizeof(ELEM_TYPE)
    //#define VECTOR_BYTE_SIZE(O) O -> size * sizeof(ELEM_TYPE)
#endif


//CONSTRUCTORS
MatrixDS* CreateMatrix(uint16_t rows, uint16_t columns, bool init) {
    MatrixDS* O = (MatrixDS*) allocate_memory(sizeof(MatrixDS));

    O -> rows = rows;
    O -> columns = columns;
    
    #if ELEM_BIT_SIZE == 4
        O -> M = (NibbleElem*) allocate_memory(MATRIX_BYTE_SIZE(O));
    #else
        O -> M = (ELEM_TYPE*) allocate_memory(MATRIX_BYTE_SIZE(O));
    #endif

    if (init)
        memset(O -> M, 0, MATRIX_BYTE_SIZE(O));

    O -> is_memory_static = false;

    return O;
}

MatrixDS* CreateMatrixFromArray(ELEM_TYPE* array, uint16_t rows, uint16_t columns, bool copy) {
    MatrixDS* O = (MatrixDS*) allocate_memory(sizeof(MatrixDS));

    O -> rows = rows;
    O -> columns = columns;

    if (copy) {
        #if ELEM_BIT_SIZE == 4
            O -> M = (NibbleElem*) allocate_memory(MATRIX_BYTE_SIZE(O));
        #else
            O -> M = (ELEM_TYPE*) allocate_memory(MATRIX_BYTE_SIZE(O));
        #endif

        memcpy(O -> M, array, MATRIX_BYTE_SIZE(O));
    } else {
        #if ELEM_BIT_SIZE == 4
            O -> M = (NibbleElem*) array;
        #else
            O -> M = array;
        #endif
    }

    O -> is_memory_static = !copy;

    return O;
}

MatrixDS* CopyMatrix(const MatrixDS* A) {
    MatrixDS* O = (MatrixDS*) allocate_memory(sizeof(MatrixDS));

    O -> rows = A -> rows;
    O -> columns = A -> columns;

    #if ELEM_BIT_SIZE == 4
        O -> M = (NibbleElem*) allocate_memory(MATRIX_BYTE_SIZE(O));
    #else
        O -> M = (ELEM_TYPE*) allocate_memory(MATRIX_BYTE_SIZE(O));
    #endif

    memcpy(O -> M, A -> M, MATRIX_BYTE_SIZE(O));

    O -> is_memory_static = false;

    return O;
}

void fill_matrix_randomly(MatrixDS* A) {
    fill_buffer_randomly(A -> M, MATRIX_BYTE_SIZE(A));
}

uint16_t nrows(const MatrixDS* A) {
    return A -> rows;
}

uint16_t ncolumns(const MatrixDS* A) {
    return A -> columns;
}

uint32_t size(const MatrixDS* A) {
    return A -> rows * A -> columns;
}

ELEM_TYPE get(const MatrixDS* A, uint16_t i, uint16_t j) {
    if (i >= A -> rows) {
        fprintf(stderr, "Row index %u out of bounds for access in matrix of size %ux%u\n", i, A -> rows, A -> columns);
        exit(-1);
    }

    if (j >= A -> columns) {
        fprintf(stderr, "Column index %u out of bounds for access in matrix of size %ux%u\n", j, A -> rows, A -> columns);
        exit(-1);
    }

    uint32_t offset = i * A -> columns + j;

    #if ELEM_BIT_SIZE == 4
        return GET_BY_OFFSET(A, offset);
    #else
        return A -> M[offset];
    #endif
}

//set
void set(MatrixDS* A, uint16_t i, uint16_t j, ELEM_TYPE v) {
    if (i >= A -> rows) {
        fprintf(stderr, "Row index %u out of bounds for set in matrix of size %ux%u\n", i, A -> rows, A -> columns);
        exit(-1);
    }

    if (j >= A -> columns) {
        fprintf(stderr, "Column index %u out of bounds for set in matrix of size %ux%u\n", j, A -> rows, A -> columns);
        exit(-1);
    }

    uint32_t offset = i * A -> columns + j;

    #if ELEM_BIT_SIZE == 4
        if ((offset & 0x1) == 0)
            (A -> M[offset >> 1]).high_nibble = v; //set just the high nibble
        else
            (A -> M[offset >> 1]).low_nibble = v; //set just the low nibble
    #else
        A -> M[offset] = v;
    #endif
}

//Product
MatrixDS* dot_product(const MatrixDS *A, const MatrixDS *B) {
    if (A -> columns != B -> rows) {
        fprintf(stderr, "Incompatible sizes for product between matrices of size %ux%u and %ux%u\n", A -> rows, A -> columns, B -> rows, B -> columns);
        exit(-1);
    }
    
    uint16_t n = A -> rows, m = B -> columns, p = A -> columns;

    MatrixDS *O = CreateMatrix(n, m, true);
    
    #if ELEM_BIT_SIZE == 4
        for (uint16_t i = 0; i < n; i++)
            for (uint16_t j = 0; j < m; j++) {
                uint32_t offset = i * O -> columns + j;
                uint32_t index = offset >> 1;

                uint32_t offset1 = j;

                if ((offset & 0x1) == 0)
                    for (uint32_t offset0 = i * p; offset0 < (i+1) * p; offset0++) {
                        (O -> M[index]).high_nibble = ELEM_ADD((O -> M[index]).high_nibble, ELEM_MUL(GET_BY_OFFSET(A, offset0), GET_BY_OFFSET(B, offset1)));
                        offset1 += B -> columns;
                    }
                else
                    for (uint32_t offset0 = i * p; offset0 < (i+1) * p; offset0++) {
                        (O -> M[index]).low_nibble = ELEM_ADD((O -> M[index]).low_nibble, ELEM_MUL(GET_BY_OFFSET(A, offset0), GET_BY_OFFSET(B, offset1)));
                        offset1 += B -> columns;
                    }
            }

    #else
        for (uint16_t i = 0; i < n; i++)
            for (uint16_t j = 0; j < m; j++)
                for (uint16_t h = 0; h < p; h++) {
                    uint32_t output_offset = i*m + j;
                    O -> M[output_offset] = ELEM_ADD(O -> M[output_offset], ELEM_MUL(A -> M[i*p + h], B -> M[h*m + j]));
                }

    #endif

    return O;
}

ELEM_TYPE row_vector_product(const MatrixDS* A, const MatrixDS* v, uint16_t row_index) {
    uint16_t vector_size = nrows(v);

    if (A -> columns != vector_size) {
        fprintf(stderr, "Incompatible sizes for ? product between matrix of size %ux%u and vector of size %u\n", A -> rows, A -> columns, vector_size);
        exit(-1);
    }

    #if ELEM_BIT_SIZE == 4
        //we're using just the high nibble to store the result
        NibbleElem result = {.high_nibble = 0, .low_nibble = 0};
        uint32_t row_offset = row_index * vector_size;

        for (uint16_t j = 0; j < vector_size; j++) {
            uint32_t matrix_offset = row_offset + j;
            result.high_nibble = ELEM_ADD(result.high_nibble, ELEM_MUL(GET_BY_OFFSET(A, matrix_offset), GET_BY_OFFSET(v, j)));
        }

        return result.high_nibble;
        
    #else
        ELEM_TYPE result = 0;
        uint32_t row_offset = row_index * A -> columns;
        
        for (uint16_t j = 0; j < vector_size; j++)            
            result = ELEM_ADD(result, ELEM_MUL(A -> M[row_offset + j], v -> M[j]));

        return result;

    #endif
}

bool are_matrices_equal(const MatrixDS* A, const MatrixDS* B) {
    if (A -> rows != B -> rows || A -> columns != B -> columns)
        return false;
    
    return memcmp(A -> M, B -> M, MATRIX_BYTE_SIZE(A)) == 0;
}

bool has_full_rank(const MatrixDS* A) {
    MatrixDS* A_ref = CopyMatrix(A);

    for (uint16_t k = 0; k < A_ref -> rows; k++) {
        // Initialize pivot row index and pivot value
        ELEM_TYPE pivot = get(A_ref, k, k); //don't use get
        uint16_t pivot_row_index = k;
        
        // look for maximum (absolute) pivot value on column k (after row k)
        for (uint16_t i = k+1; i < A_ref -> rows; i++)
            if (get(A_ref, i, k) > pivot) //should be abs and don't use get
                pivot = get(A_ref, i, k), pivot_row_index = i; //don't use get

        // if a principal diagonal element is zero, then the
        // matrix is singular, so it can't have full rank
        if (!get(A_ref, k, pivot_row_index)) {//don't use get
            destroy_matrix(A_ref);
            return false;
        }

        // Swap the pivot row with current row
        if (pivot_row_index != k) {
            //uint32_t row_offset0 = pivot_row_index, row_offset1 = 0;

            for (uint16_t j = 0; j < A_ref -> columns; j++) {
                ELEM_TYPE tmp = get(A_ref, pivot_row_index, j);
                set(A_ref, pivot_row_index, j, get(A_ref, k, j));
                set(A_ref, k, j, tmp);
            }
        }

        for (uint16_t i = k+1; i < A_ref -> rows; i++) {
            // current_row = pivot * current_row - current_row[0] * pivot_row
            for (uint16_t j = k+1; j < A_ref -> columns; j++)
                set(A_ref, i, j, ELEM_SUB(ELEM_MUL(pivot, get(A_ref, i, j)), ELEM_MUL(get(A_ref, i, k), get(A_ref, k, j))));

            // set to zero the current column value
            set(A_ref, i, k, 0);
        }
    }

    destroy_matrix(A_ref);
    
    return true;
}

void print_matrix(const MatrixDS* A) {
    for (uint16_t i = 0; i < A -> rows; i++) {
        for (uint16_t j = 0; j < A -> columns; j++) 
            printf("%u ", get(A, i, j)); //fix
        printf("\n");
    }
}

//DESTRUCTOR
void destroy_matrix(MatrixDS* A) {
    if (!A -> is_memory_static)
        free_memory(A -> M);
    
    free_memory(A);
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