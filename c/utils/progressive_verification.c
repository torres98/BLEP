#ifndef PROGRESSIVE_VERIFICATION_H
#define PROGRESSIVE_VERIFICATION_H

#include <stdio.h>
#include <stdlib.h>

#include "../include/progressive_verification.h"

#include "../include/memory_utils.h"
#include "../include/random_utils.h"
#include "../include/math_utils.h" //dunno why...

bool progVer(const MatrixDS* M, const MatrixDS* v, uint16_t t, bool rand) {

    uint16_t pk_nrows = nrows(M), sig_size = nrows(v);

    if (ncolumns(M) != sig_size) {
        fprintf(stderr, "Incompatible sizes for progressive signature verification (public key of size %ux%u and signature of size %u)", pk_nrows, ncolumns(M), sig_size);
        exit(-1);
    }

    if (t > pk_nrows) {
        fprintf(stderr, "The number of steps (%u) can't be bigger than the number of rows of the matrix (%u)", t, pk_nrows);
        exit(-1);
    }

    if (t == 0)
        return false;

    bool verification_result = true;

    if (rand) {
        uint16_t* row_indices = (uint16_t*) allocate_memory(pk_nrows * sizeof(uint16_t));

        for (uint16_t i = 0; i < pk_nrows; i++)
            row_indices[i] = i;

        shuffle_array(row_indices, pk_nrows);

        for (uint16_t i = 0; i < t; i++) {
            ELEM_TYPE row_column_product = row_vector_product(M, v, row_indices[i]);

            if (row_column_product != 0)
                verification_result = false;
        }

        free_memory(row_indices);
    } else {
        for (uint16_t i = 0; i < t; i++) {
            ELEM_TYPE row_column_product = row_vector_product(M, v, i);

            if (row_column_product != 0)
                verification_result = false;
        }
    }

    return verification_result;
}

bool progVer_with_result_vector(const MatrixDS* PK, const MatrixDS* s, const MatrixDS* u, uint16_t t, bool rand) {

    uint16_t pk_nrows = nrows(PK), sig_size = nrows(s);

    if (ncolumns(PK) != sig_size) {
        fprintf(stderr, "Incompatible sizes for progressive signature verification (public key of size %ux%u and signature of size %u)", pk_nrows, ncolumns(PK), sig_size);
        exit(-1);
    }

    if (pk_nrows != nrows(u)) {
        fprintf(stderr, "Incompatible sizes for signature verification (public key of size %ux%u and resulting vector of size %u)\n", pk_nrows, ncolumns(PK), nrows(u));
        exit(-1);
    }

    if (t > pk_nrows) {
        fprintf(stderr, "The number of steps (%u) can't be bigger than the number of rows of the public key (%u)", t, pk_nrows);
        exit(-1);
    }

    if (t == 0)
        return false;

    bool verification_result = true;

    if (rand) {
        uint16_t* row_indices = (uint16_t*) allocate_memory(pk_nrows * sizeof(uint16_t));

        for (uint16_t i = 0; i < pk_nrows; i++)
            row_indices[i] = i;
        
        shuffle_array(row_indices, pk_nrows);
        
        for (uint16_t i = 0; i < t; i++) {
            ELEM_TYPE row_column_product = row_vector_product(PK, s, row_indices[i]);
            
            if (row_column_product != get(u, row_indices[i], 0))
                verification_result = false;
        }

        free_memory(row_indices);
    } else {
        for (uint16_t i = 0; i < t; i++) {
            ELEM_TYPE row_column_product = row_vector_product(PK, s, i);
            
            if (row_column_product != get(u, i, 0))
                verification_result = false;
        }
    }

    return verification_result;
}

#endif
