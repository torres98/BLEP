#include <cassert>
#include <chrono>
#include <tuple>

#include "../include/random_utils.h"
#include "../include/rainbow.h"
#include "../include/standard_verification.h"
#include "../include/efficient_verification.h"
#include "../include/progressive_verification.h"

#define STR_IMPL_(x) #x
#define STR(x) STR_IMPL_(x)

char const *pk_path = "/home/torres/Desktop/Thesis/verification_implementation/tmp/pk" STR(RAINBOW_VERSION) ".txt";

unsigned char salt[Rainbow::SALT_SIZE];

using Rainbow::gf;
using std::chrono::high_resolution_clock;
using std::chrono::duration;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Wrong number of arguments." << std::endl;
        return -1;
    }

    uint16_t k = strtoul(argv[1], NULL, 10);
    uint32_t SAMPLE_SIZE = strtoul(argv[2], NULL, 10);
    
    if (k > Rainbow::n_polynomials) {
        std::cerr << "The number of rows of the SVK (" << k << ") can't be bigger than the number of rows of the PK (" << Rainbow::n_polynomials << ")" << std::endl;
        return -1;
    }

    MatrixDS<gf> PK = Rainbow::parse_public_key(pk_path);
    long double offver_time = .0;

    for (uint32_t i = 0; i < SAMPLE_SIZE; i++) {
        auto starting_time = high_resolution_clock::now();
        auto [C, svk] = offVer(PK, k);
        offver_time += duration<long double, std::micro>(high_resolution_clock::now() - starting_time).count();
    }

    std::cout << "Average offVer time (" << k << " SVK rows): " << offver_time / SAMPLE_SIZE << " microseconds" << std::endl;

    return 0;
}
