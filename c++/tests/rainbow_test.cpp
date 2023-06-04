#include <cassert>

#include "blep/utils/rand.h"
#include "blep/schemes/rainbow.h"
#include "blep/mv_verification/std_ver.h"

#define STR_IMPL_(x) #x
#define STR(x) STR_IMPL_(x)

char const *pk_path = STR(PROJECT_DIR) "/tmp/pk" STR(RAINBOW_VERSION) ".txt";
char const *signature_path = STR(PROJECT_DIR) "/tmp/signature" STR(RAINBOW_VERSION) ".txt";
char const *message_path = STR(PROJECT_DIR) "/tmp/debug.gdb";

using Rainbow::gf;

int main() {
    unsigned char salt[Rainbow::SALT_SIZE];

    MatrixDS<gf> PK = Rainbow::get_public_key_from_file(pk_path);
    VectorDS<gf> s = Rainbow::get_signature_from_file(signature_path, salt);

    VectorDS<gf> u = Rainbow::get_result_vector_from_file(message_path, salt);

    assert(verify_signature(PK, s, u));
    std::cout << "Successfully verified the Rainbow signature." << std::endl;

    return 0;
}
