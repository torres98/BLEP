#ifndef RAND_H
#define RAND_H

#include <stddef.h>
#include <stdint.h>


void fill_buffer_randomly(void* buffer, size_t size);
void shuffle_array(uint16_t* arr, size_t n);

#endif
