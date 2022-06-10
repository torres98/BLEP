#include <iostream>


struct Foo {
    uint8_t v;
};

int main() {

    std::cout << "Eccoci qua\n";
    unsigned int SIZE = 435184;

    Foo* p = new Foo[SIZE];

    p[SIZE-1].v = 104;
    std::cout << p[SIZE-1].v << ": " << p << " - " << &(p[SIZE-1]) << " = " << p - &(p[SIZE-1]) << std::endl;

    delete[] p;
    return 0;
}
