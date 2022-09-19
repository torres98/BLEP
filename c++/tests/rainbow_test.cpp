#include <cassert>

#include "../include/random_utils.h"
#include "../include/rainbow.h"
#include "../include/standard_verification.h"
#include "../include/efficient_verification.h"
#include "../include/progressive_verification.h"

#define STR_IMPL_(x) #x
#define STR(x) STR_IMPL_(x)

char const *pk_path = "/home/torres/Desktop/Thesis/verification_implementation/tmp/pk" STR(RAINBOW_VERSION) ".txt";
char const *signature_path = "/home/torres/Desktop/Thesis/verification_implementation/tmp/signature" STR(RAINBOW_VERSION) ".txt";
char const *message_path = "/home/torres/Desktop/Thesis/verification_implementation/tmp/debug.gdb";

using Rainbow::gf;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Wrong number of arguments.\n");
        exit(-1);
    }

    uint16_t k = strtoul(argv[1], NULL, 10);
    uint16_t t = strtoul(argv[2], NULL, 10);
    
    if (k > Rainbow::n_polynomials) {
        std::cerr << "The number of rows of the SVK (" << k << ") can't be bigger than the number of rows of the PK (" << Rainbow::n_polynomials << ")" << std::endl;
        return -1;
    } else if (t > k) {
        std::cerr << "The number of progressive steps (" << t << ") can't be bigger than the number of rows of the SVK (" << k << ")" << std::endl;
        return -1;
    }

    unsigned char salt[Rainbow::SALT_SIZE];

    MatrixDS<gf> PK = Rainbow::parse_public_key(pk_path);
    VectorDS<gf> s = Rainbow::parse_signature(signature_path, salt);

    VectorDS<gf> u = Rainbow::get_result_vector(message_path, salt);

    assert(verify_signature(PK, s, u));
    std::cout << "Successfully verified the Rainbow signature." << std::endl;

    // for efficient verification
    auto [C, svk] = offVer(PK, k);
    VectorDS<gf> u_eff = (VectorDS<gf>) (C * u);

    assert(verify_signature(svk, s, u_eff));
    std::cout << "Efficiently verified the Rainbow signature." << std::endl;

    assert(progVer(PK, s, u, t));
    std::cout << "Progressively verified the Rainbow signature." << std::endl;

    assert(progVer(svk, s, u_eff, t));
    std::cout << "Efficiently and progressively verified the Rainbow signature." << std::endl;

    return 0;
}
