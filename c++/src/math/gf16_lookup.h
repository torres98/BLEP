#if GF16_LOOKUP == 3 || GF16_LOOKUP == 4
    #if GF16_LOOKUP == 3
        const
    #endif
    unsigned char gf16_add_lookup[16][16] = {
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
        {1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14},
        {2, 3, 0, 1, 6, 7, 4, 5, 10, 11, 8, 9, 14, 15, 12, 13},
        {3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12},
        {4, 5, 6, 7, 0, 1, 2, 3, 12, 13, 14, 15, 8, 9, 10, 11},
        {5, 4, 7, 6, 1, 0, 3, 2, 13, 12, 15, 14, 9, 8, 11, 10},
        {6, 7, 4, 5, 2, 3, 0, 1, 14, 15, 12, 13, 10, 11, 8, 9},
        {7, 6, 5, 4, 3, 2, 1, 0, 15, 14, 13, 12, 11, 10, 9, 8},
        {8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7},
        {9, 8, 11, 10, 13, 12, 15, 14, 1, 0, 3, 2, 5, 4, 7, 6},
        {10, 11, 8, 9, 14, 15, 12, 13, 2, 3, 0, 1, 6, 7, 4, 5},
        {11, 10, 9, 8, 15, 14, 13, 12, 3, 2, 1, 0, 7, 6, 5, 4},
        {12, 13, 14, 15, 8, 9, 10, 11, 4, 5, 6, 7, 0, 1, 2, 3},
        {13, 12, 15, 14, 9, 8, 11, 10, 5, 4, 7, 6, 1, 0, 3, 2},
        {14, 15, 12, 13, 10, 11, 8, 9, 6, 7, 4, 5, 2, 3, 0, 1},
        {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0}
    };

    #if GF16_LOOKUP == 4
        unsigned char gf16_mul_lookup[16][16] = {
    #else
        const unsigned char gf16_mul_lookup[16][16] = {
    #endif
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
        {0, 2, 3, 1, 8, 10, 11, 9, 12, 14, 15, 13, 4, 6, 7, 5},
        {0, 3, 1, 2, 12, 15, 13, 14, 4, 7, 5, 6, 8, 11, 9, 10},
        {0, 4, 8, 12, 6, 2, 14, 10, 11, 15, 3, 7, 13, 9, 5, 1},
        {0, 5, 10, 15, 2, 7, 8, 13, 3, 6, 9, 12, 1, 4, 11, 14},
        {0, 6, 11, 13, 14, 8, 5, 3, 7, 1, 12, 10, 9, 15, 2, 4},
        {0, 7, 9, 14, 10, 13, 3, 4, 15, 8, 6, 1, 5, 2, 12, 11},
        {0, 8, 12, 4, 11, 3, 7, 15, 13, 5, 1, 9, 6, 14, 10, 2},
        {0, 9, 14, 7, 15, 6, 1, 8, 5, 12, 11, 2, 10, 3, 4, 13},
        {0, 10, 15, 5, 3, 9, 12, 6, 1, 11, 14, 4, 2, 8, 13, 7},
        {0, 11, 13, 6, 7, 12, 10, 1, 9, 2, 4, 15, 14, 5, 3, 8},
        {0, 12, 4, 8, 13, 1, 9, 5, 6, 10, 2, 14, 11, 7, 15, 3},
        {0, 13, 6, 11, 9, 4, 15, 2, 14, 3, 8, 5, 7, 10, 1, 12},
        {0, 14, 7, 9, 5, 11, 2, 12, 10, 4, 13, 3, 15, 1, 8, 6},
        {0, 15, 5, 10, 1, 14, 4, 11, 2, 13, 7, 8, 3, 12, 6, 9}
    };

#elif GF16_LOOKUP == 1 || GF16_LOOKUP == 2
    #if GF16_LOOKUP == 2
        unsigned char gf16_add_lookup[] = {
    #else
        const unsigned char gf16_add_lookup[] = {
    #endif
        1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 
            3,  2,  5,  4,  7,  6,  9,  8, 11, 10, 13, 12, 15, 14, 
                1,  6,  7,  4,  5, 10, 11,  8,  9, 14, 15, 12, 13, 
                    7,  6,  5,  4, 11, 10,  9,  8, 15, 14, 13, 12, 
                        1,  2,  3, 12, 13, 14, 15,  8,  9, 10, 11, 
                            3,  2, 13, 12, 15, 14,  9,  8, 11, 10, 
                                1, 14, 15, 12, 13, 10, 11,  8,  9, 
                                   15, 14, 13, 12, 11, 10,  9,  8, 
                                        1,  2,  3,  4,  5,  6,  7, 
                                            3,  2,  5,  4,  7,  6, 
                                                1,  6,  7,  4,  5, 
                                                    7,  6,  5,  4, 
                                                        1,  2,  3, 
                                                            3,  2, 
                                                                1                                                             
    };

    #if GF16_LOOKUP == 2
        unsigned char gf16_mul_lookup[] = {
    #else
        const unsigned char gf16_mul_lookup[] = {
    #endif
        1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 
            3,  1,  8, 10, 11,  9, 12, 14, 15, 13,  4,  6,  7,  5, 
                2, 12, 15, 13, 14,  4,  7,  5,  6,  8, 11,  9, 10, 
                    6,  2, 14, 10, 11, 15,  3,  7, 13,  9,  5,  1, 
                        7,  8, 13,  3,  6,  9, 12,  1,  4, 11, 14, 
                            5,  3,  7,  1, 12, 10,  9, 15,  2,  4, 
                                4, 15,  8,  6,  1,  5,  2, 12, 11, 
                                   13,  5,  1,  9,  6, 14, 10,  2, 
                                       12, 11,  2, 10,  3,  4, 13, 
                                           14,  4,  2,  8, 13,  7, 
                                               15, 14,  5,  3,  8, 
                                                    11,  7, 15, 3, 
                                                       10,  1, 12, 
                                                            8,  6, 
                                                                9 
    };

#endif
//const unsigned char gf16_inv_lookup[15] = {1, 3, 2, 15, 12, 9, 11, 10, 6, 8, 7, 5, 14, 13, 4};
