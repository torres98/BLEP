#include <iostream>

#include <kernel.h>

#include "tiny/include/uart_utils.h"

#define SIZE 4096*3
unsigned char buffer[SIZE];

int main() {

    read_n_bytes(uart_dev, buffer, SIZE);

    std::cout << buffer[0] << "\n";

    return 0;
}