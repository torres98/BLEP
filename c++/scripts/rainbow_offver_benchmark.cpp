#include <chrono>

#include "blep/mv_verification/std_ver.h"
#include "blep/mv_verification/eff_ver.h"
#include "blep/mv_verification/prog_ver.h"
#include "blep/schemes/rainbow.h"
#include "blep/utils/rand.h"

#if RAINBOW_VERSION == 1
    #define RAINBOW_VERSION_STR "I"
    #define GF_LOOKUP GF16_LOOKUP
#elif RAINBOW_VERSION == 2
    #define RAINBOW_VERSION_STR "III"
    #define GF_LOOKUP GF256_LOOKUP
#else
    #define RAINBOW_VERSION_STR "V"
    #define GF_LOOKUP GF256_LOOKUP
#endif

#define STR_IMPL_(x) #x
#define STR(x) STR_IMPL_(x)

char const *pk_path = STR(PROJECT_DIR) "/../rainbow_examples/pk" STR(RAINBOW_VERSION) ".txt";

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

    MatrixDS<gf> PK = Rainbow::get_public_key_from_file(pk_path);
    long double offver_time = .0;

    for (uint32_t i = 0; i < SAMPLE_SIZE; i++) {
        auto starting_time = high_resolution_clock::now();
        auto [C, svk] = offVer(PK, k);
        offver_time += duration<long double, std::micro>(high_resolution_clock::now() - starting_time).count();
    }
    
    std::cout << "Rainbow " << RAINBOW_VERSION_STR << " offVer benchmark (lookup level " << GF_LOOKUP << ")" << std::endl;
    std::cout << "Average offVer time (" << k << " SVK rows): " << offver_time / SAMPLE_SIZE << " microseconds" << std::endl;

    return 0;
}
