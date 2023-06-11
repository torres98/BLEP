#include <stdio.h>
#include <stdlib.h>

#include "blep/utils/memory.h"

void* allocate_memory(size_t size) {
    void* p = malloc(size);
    
    if (p == NULL) {
        fprintf(stderr, "Bad alloc.\n");
        exit(-1);
    }

    return p;
}

void free_memory(void* p) {
    free(p);
}
