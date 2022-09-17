#ifndef MEMORY_UTILS_H
#define MEMORY_UTILS_H

#include <stddef.h>


void* allocate_memory(size_t size);
void free_memory(void* p);

#endif
