#include <cassert>
#include <chrono>
#include <tuple>

#include "blep/mv_verification/std_ver.h"
#include "blep/mv_verification/eff_ver.h"
#include "blep/mv_verification/prog_ver.h"
#include "blep/schemes/rainbow.h"
#include "blep/utils/rand.h"


#if RAINBOW_VERSION == 1
    #define SIG_SIZE Rainbow::n_variables / 2
    #define RB_VER "I"
#elif RAINBOW_VERSION == 2
    #define SIG_SIZE Rainbow::n_variables
    #define RB_VER "III"
#else
    #define SIG_SIZE Rainbow::n_variables
    #define RB_VER "V"
#endif

#define STR_IMPL_(x) #x
#define STR(x) STR_IMPL_(x)

char const *pk_path = STR(PROJECT_DIR) "/tmp/pk" STR(RAINBOW_VERSION) ".txt";
char const *signature_path = STR(PROJECT_DIR) "/tmp/signature" STR(RAINBOW_VERSION) ".txt";
char const *message_path = STR(PROJECT_DIR) "/tmp/debug.gdb";

unsigned char salt[Rainbow::SALT_SIZE];

using Rainbow::gf;
using std::chrono::high_resolution_clock;
using std::chrono::duration;

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Expected 3 args: k (#svk rows) t (#progressive steps) s (sample size)." << std::endl;
        return -1;
    }

    uint16_t k = strtoul(argv[1], NULL, 10), t = strtoul(argv[2], NULL, 10);
    uint32_t SAMPLE_SIZE = strtoul(argv[3], NULL, 10);
    
    if (k > Rainbow::n_polynomials) {
        std::cerr << "The number of rows of the SVK (" << k << ") can't be bigger than the number of rows of the PK (" << Rainbow::n_polynomials << ")" << std::endl;
        return -1;
    } else if (t > k) {
        std::cerr << "The number of progressive steps (" << t << ") can't be bigger than the number of rows of the SVK (" << k << ")" << std::endl;
        return -1;
    }

    MatrixDS<gf> PK = Rainbow::get_public_key_from_file(pk_path);
    auto [C, SVK] = offVer(PK, k);

    uint32_t eff_error_count = 0, prog_error_count = 0, effprog_error_count = 0;
    long double eff_time = .0, prog_time = .0, effprog_time = .0;

    for (uint32_t i = 0; i < SAMPLE_SIZE; i++) {
        //fill signature randomly
        unsigned char signature_raw_str[SIG_SIZE];
        fill_buffer_randomly(signature_raw_str, SIG_SIZE);
        VectorDS<gf> signature_guess = Rainbow::get_signature(signature_raw_str);

        // fill salt randomly
        unsigned char salt[Rainbow::SALT_SIZE];
        fill_buffer_randomly(salt, Rainbow::SALT_SIZE);
        VectorDS<gf> u = Rainbow::get_result_vector_from_file(message_path, salt);

        VectorDS<gf> u_eff = C * u;
        
        auto starting_time = high_resolution_clock::now();
        bool eff_verification_result = verify_signature(SVK, signature_guess, u_eff);
        eff_time += duration<long double, std::micro>(high_resolution_clock::now() - starting_time).count();

        starting_time = high_resolution_clock::now();
        bool prog_verification_result = progVer(PK, signature_guess, u, t);
        prog_time += duration<long double, std::micro>(high_resolution_clock::now() - starting_time).count();

        starting_time = high_resolution_clock::now();
        bool effprog_verification_result = progVer(SVK, signature_guess, u_eff, t);
        effprog_time += duration<long double, std::micro>(high_resolution_clock::now() - starting_time).count();

        if (eff_verification_result || prog_verification_result || effprog_verification_result) {
            if (!verify_signature(PK, signature_guess, u)) {
                if (eff_verification_result)
                    eff_error_count++;
                
                if (prog_verification_result)
                    prog_error_count++;

                if (effprog_verification_result)
                    effprog_error_count++;
            }
        }
    }

    std::cout << "Rainbow " RB_VER " verification benchmark\n" << std::endl ;

    std::cout << "EFFICIENT VERIFICATION STATS (" << k << " SVK rows)" << std::endl;
    std::cout << "Error percentage: " << (long double) eff_error_count / SAMPLE_SIZE * 100 << "%" << std::endl;
    std::cout << "Security bits: " << ((eff_error_count == 0) ? 0: log2((long double) eff_error_count / SAMPLE_SIZE)) << std::endl;
    std::cout << "Average verification time: " << eff_time / SAMPLE_SIZE << " microseconds" << std::endl << std::endl;

    std::cout << "PROGRESSIVE VERIFICATION STATS (" << t << " progressive steps)" << std::endl;
    std::cout << "Error percentage: " << (long double) prog_error_count / SAMPLE_SIZE * 100 << "%" << std::endl;
    std::cout << "Security bits: " << ((prog_error_count == 0) ? 0: log2((long double) prog_error_count / SAMPLE_SIZE)) << std::endl;
    std::cout << "Average verification time: " << prog_time / SAMPLE_SIZE << " microseconds" << std::endl << std::endl;

    std::cout << "EFFICIENT+PROGRESSIVE VERIFICATION STATS (" << k << " SVK rows, " << t << " progressive steps)" << std::endl;
    std::cout << "Error percentage: " << (long double) effprog_error_count / SAMPLE_SIZE * 100 << "%" << std::endl;
    std::cout << "Security bits: " << ((effprog_error_count == 0) ? 0: log2((long double) effprog_error_count / SAMPLE_SIZE)) << std::endl;
    std::cout << "Average verification time: " << effprog_time / SAMPLE_SIZE << " microseconds" << std::endl;

    return 0;
}
