#include <iostream>

struct Foo {
    uint8_t v;
};

int main() {

    std::cout << "Eccoci qua\n";
    unsigned int SIZE = 5000;

    for (unsigned int i = 0; i < 10; i++) {
        Foo* p = new Foo[SIZE];

        p[SIZE-1].v = 104;
        std::cout << p << std::endl;
        delete[] p;
    }

    std::cout << "FINE\n";
    return 0;
}
