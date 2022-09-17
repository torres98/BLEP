#include <stdlib.h>

#include "../include/file_utils.h"


FILE* open_file(const char* filename, const char* modes) {
    FILE* f = fopen(filename, modes);

    if (f == NULL) {
        fprintf(stderr, "Error during opening of file %s (modes %s)\n", filename, modes);   
        exit(-1);
    }
    
    return f;
}

size_t get_file_size(FILE* f) {
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);

    if (file_size == -1L) {
        fprintf(stderr, "An error occurred while reading file size.\n");
        exit(-1);
    }

    rewind(f);

    return (size_t) file_size;
}
