#include "blep/schemes/rainbow.h"
#include "blep/mv_verification/std_ver.h"
#include "blep/mv_verification/eff_ver.h"
#include "blep/mv_verification/prog_ver.h"

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

#define assert(e) if ((e) == 0) { std::cout << "FAILED" << std::endl; return 1; }
#define STR_IMPL_(x) #x
#define STR(x) STR_IMPL_(x)

char const *pk_path = STR(PROJECT_DIR) "/../rainbow_examples/pk" STR(RAINBOW_VERSION) ".txt";
char const *signature_path = STR(PROJECT_DIR) "/../rainbow_examples/signature" STR(RAINBOW_VERSION) ".txt";
char const *message_path = STR(PROJECT_DIR) "/../rainbow_examples/message.txt";

using Rainbow::gf;

unsigned char salt[Rainbow::SALT_SIZE];

int main() {
    std::cout << "Rainbow " RAINBOW_VERSION_STR " Standard Verification Test: ";

    MatrixDS<gf> PK = Rainbow::get_public_key_from_file(pk_path);
    VectorDS<gf> s = Rainbow::get_signature_from_file(signature_path, salt);

    VectorDS<gf> u = Rainbow::get_result_vector_from_file(message_path, salt);

    assert(verify_signature(PK, s, u));
    std::cout << "PASSED" << std::endl;

    // for efficient verification
    std::cout << "Rainbow " RAINBOW_VERSION_STR " Efficient Verification Test: ";
    uint16_t k = Rainbow::n_polynomials / 2;
    auto [C,  SVK] = offVer(PK, k);
    VectorDS<gf> u_eff = C * u;

    assert(verify_signature(SVK, s, u_eff));
    std::cout << "PASSED" << std::endl;

    // for progressive verification
    std::cout << "Rainbow " RAINBOW_VERSION_STR " Progressive Verification Test: ";
    uint16_t t = Rainbow::n_polynomials / 2;
    assert(progVer(PK, s, u, t, true));
    std::cout << "PASSED" << std::endl;

    std::cout << "Rainbow " RAINBOW_VERSION_STR " Efficient+Progressive Verification Test: ";
    assert(progVer(SVK, s, u_eff, t, true));
    std::cout << "PASSED" << std::endl;

    return 0;
}
