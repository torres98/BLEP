#include <stdio.h>
#include <stdlib.h>

#include "../include/random_utils.h"
#include "../include/rainbow.h"
#include "../include/standard_verification.h"
//#include "../include/efficient_verification.h"

char const *pk_path = "/home/torres/Desktop/Thesis/verification_implementation/tmp/pk.txt";


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Wrong number of arguments\n");
        exit(-1);
    }

    uint16_t k = strtoul(argv[1], NULL, 10);

    if (k > n_polynomials) {
        fprintf(stderr, "The number of rows of the SVK (%u) can't be bigger than the number of rows of the PK (%u)\n", k, n_polynomials);
        exit(-1);
    }

    MatrixDS* PK = parse_public_key(pk_path);
    MatrixDS* C = CreateMatrix(k, nrows(PK), false);
    fill_matrix_randomly(C);
    MatrixDS* SVK = dot_product(C, PK);
    
    //FILE *f = fopen("src/svk.h", "w");

    #if 0
        fprintf(f, "#include <stdint.h>\n\nconst uint8_t short_private_key[%u] = {", size(SVK) >> 1);

        for (uint16_t i = 0; i < nrows(SVK); i++)
            for (uint16_t j = 0; j < ncolumns(SVK); j+=2)
                fprintf(f, "%u, ", (get(SVK, i, j+1) << 4) | (get(SVK, i, j)));
                
        fprintf(f, "};\n\nconst uint8_t private_transformation[%u] = {", size(C) >> 1);

        for (uint16_t i = 0; i < nrows(C); i++)
            for (uint16_t j = 0; j < ncolumns(C); j+=2)
                fprintf(f, "%u, ", (get(C, i, j+1) << 4) | (get(C, i, j)));

    #else
        //fprintf(f, "#include <stdint.h>\n\nconst uint8_t short_private_key[%u] = {", size(SVK));
        printf("#include <stdint.h>\n\nconst uint8_t short_private_key[%u] = {", size(SVK));

        for (uint16_t i = 0; i < nrows(SVK); i++)
            for (uint16_t j = 0; j < ncolumns(SVK); j++)
                printf("%u, ", get(SVK, i, j));
                //fprintf(f, "%u, ", get(SVK, i, j));
                
        //fprintf(f, "};\n\nconst uint8_t private_transformation[%u] = {", size(C) >> 1);
        printf("};\n\nconst uint8_t private_transformation[%u] = {", size(C) >> 1);

        for (uint16_t i = 0; i < nrows(C); i++)
            for (uint16_t j = 0; j < ncolumns(C); j++)
                printf("%u, ", get(C, i, j));
                //fprintf(f, "%u, ", get(C, i, j));

    #endif

    //fprintf(f, "};\n");
    printf("};\n");
    //fclose(f);

    return 0;
}
