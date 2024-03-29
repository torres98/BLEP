#include <zephyr/random/rand32.h>

#include "../include/random_utils.h"


void fill_buffer_randomly(void* buffer, size_t size) {
    #ifdef CONFIG_CSPRING_ENABLED
        sys_csrand_get(buffer, size);
    #else
        sys_rand_get(buffer, size);
    #endif
}

void shuffle_array(uint16_t* arr, unsigned int n) {
    for (unsigned int i = 0; i < n - 1; i++) {
        unsigned int j = i + (sys_rand32_get() % (n - i));

        uint16_t temp = arr[j];
        arr[j] = arr[i];
        arr[i] = temp;
    }
}
