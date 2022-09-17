#include <stdlib.h>
#include <stdio.h>

#include "../include/random_utils.h"


void fill_buffer_randomly(void* buffer, size_t size) {
    FILE* f = fopen("/dev/urandom", "r");
    fread(buffer, 1, size, f); //find a way around
    fclose(f);
}

void shuffle_array(uint16_t* arr, unsigned int n) {
    for (unsigned int i = 0; i < n - 1; i++) {
        unsigned int j = i + (rand() % (n - i));

        unsigned int temp = arr[j];
        arr[j] = arr[i];
        arr[i] = temp;
    }
}
