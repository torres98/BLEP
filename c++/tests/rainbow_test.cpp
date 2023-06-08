#include <cassert>

#include "blep/utils/rand.h"
#include "blep/schemes/rainbow.h"
#include "blep/mv_verification/std_ver.h"
#include "blep/mv_verification/eff_ver.h"
#include "blep/mv_verification/prog_ver.h"

#define STR_IMPL_(x) #x
#define STR(x) STR_IMPL_(x)

char const *pk_path = STR(PROJECT_DIR) "/tmp/pk" STR(RAINBOW_VERSION) ".txt";
char const *signature_path = STR(PROJECT_DIR) "/tmp/signature" STR(RAINBOW_VERSION) ".txt";
char const *message_path = STR(PROJECT_DIR) "/tmp/debug.gdb";

using Rainbow::gf;

int main(int argc, char *argv[]) {

    if (argc != 3) {
        std::cerr << "Wrong number of arguments." << std::endl;
        exit(1);
    }

    uint16_t k = strtoul(argv[1], NULL, 10);
    uint16_t t = strtoul(argv[2], NULL, 10);
    
    if (k > Rainbow::n_polynomials) {
        std::cerr << "The number of rows of the SVK (" << k << ") can't be bigger than the number of rows of the PK (" << Rainbow::n_polynomials << ")." << std::endl;
        return 1;
    } else if (t > k) {
        std::cerr << "The number of progressive steps (" << t << ") can't be bigger than the number of rows of the SVK (" << k << ")." << std::endl;
        return 1;
    }

    unsigned char salt[Rainbow::SALT_SIZE];

    MatrixDS<gf> PK = Rainbow::get_public_key_from_file(pk_path);
    VectorDS<gf> s = Rainbow::get_signature_from_file(signature_path, salt);

    VectorDS<gf> u = Rainbow::get_result_vector_from_file(message_path, salt);

    assert(verify_signature(PK, s, u));
    std::cout << "Successfully verified the Rainbow signature." << std::endl;

    // for efficient verification
    auto [C,  SVK] = offVer(PK, k);

    VectorDS<gf> u_eff = C * u;

    assert(verify_signature(SVK, s, u_eff));
    std::cout << "Efficiently verified the Rainbow signature." << std::endl;

    // for progressive verification
    assert(progVer(PK, s, u, t, true) == true);
    std::cout << "Progressively verified the Rainbow signature." << std::endl;

    assert(progVer(SVK, s, u_eff, t, true));
    std::cout << "Efficiently and progressively verified the Rainbow signature." << std::endl;

    return 0;
}
