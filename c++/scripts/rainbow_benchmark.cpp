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
char const *signature_path = "/home/torres/Desktop/Thesis/verification_implementation/tmp/signature" STR(RAINBOW_VERSION) ".txt";
char const *message_path = "/home/torres/Desktop/Thesis/verification_implementation/tmp/debug.gdb";

unsigned char salt[Rainbow::SALT_SIZE];

using Rainbow::gf;
using std::chrono::high_resolution_clock;
using std::chrono::duration;

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Wrong number of arguments." << std::endl;
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

    MatrixDS<gf> PK = Rainbow::parse_public_key(pk_path);
    VectorDS<gf> s = Rainbow::parse_signature(signature_path, salt);

    VectorDS<gf> u = Rainbow::get_result_vector(message_path, salt);

    // for efficient verification
    auto [C, svk] = offVer(PK, k);
    VectorDS<gf> u_eff = (VectorDS<gf>) (C * u);

    VectorDS<gf> signature_guess = VectorDS<gf>(s.nrows());
    uint32_t eff_error_count = 0, prog_error_count = 0, effprog_error_count = 0;

    long double eff_time = .0, prog_time = .0, effprog_time = .0;

    for (uint32_t i = 0; i < SAMPLE_SIZE; i++) {
        fill_matrix_randomly(signature_guess, 0, Rainbow::q - 1);
        
        auto starting_time = high_resolution_clock::now();
        bool eff_verification_result = verify_signature(svk, signature_guess, u_eff);
        eff_time += duration<long double, std::micro>(high_resolution_clock::now() - starting_time).count();

        starting_time = high_resolution_clock::now();
        bool prog_verification_result = progVer(PK, signature_guess, u, t);
        prog_time += duration<long double, std::micro>(high_resolution_clock::now() - starting_time).count();

        starting_time = high_resolution_clock::now();
        bool effprog_verification_result = progVer(svk, signature_guess, u_eff, t);
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
