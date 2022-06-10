#include <iostream>

#include "flash_array.h"

int main() {
    std::cout << sizeof(unsigned int) << std::endl;

    for (int i = 0; i < 10; i++)
        std::cout << test_array[TEST_ARRAY_SIZE - i - 1] << ", ";

    std::cout << test_array[0] * test_array[1] << std::endl;
    
    return 0;
}
