#include "tiny/include/uart_utils.h"
#include <iostream>

int main() {

    uart_poll_out(uart_dev, '0');
    uart_poll_out(uart_dev, '\n');

    std::cout << "IO\n";

    return 0;

}