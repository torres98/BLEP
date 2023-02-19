#include "blep/utils/rand.h"


std::mt19937 rng((std::random_device())());

void fill_buffer_randomly(unsigned char* buffer, size_t buf_size) {
    FILE* rand_file = fopen("/dev/random", "r");

    if (fread(buffer, sizeof(unsigned char), buf_size, rand_file) != buf_size)
        throw std::runtime_error("Unexpected error when reading /dev/random");

    fclose(rand_file);
}