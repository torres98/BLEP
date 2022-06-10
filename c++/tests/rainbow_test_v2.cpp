#include <assert.h>

#include "../include/random_utils.h"
#include "../schemes/rainbow_v2.cpp"
#include "../include/standard_verification.h"
#include "../include/efficient_verification.h"
#include "../include/progressive_verification.h"

#define SAMPLE_SIZE 10000

char const *pk_path = "/home/torres/Desktop/Thesis/verification_implementation/tmp/pk.txt";
char const *signature_path = "/home/torres/Desktop/Thesis/verification_implementation/tmp/signature.txt";
char const *message_path = "/home/torres/Desktop/Thesis/verification_implementation/tmp/debug.gdb";

using Rainbow::gf;

int main(int argc, char *argv[]) {

    unsigned char salt[16];

    Matrix<gf> PK = Rainbow::parse_public_key(pk_path);
    Vector<gf> s = Rainbow::parse_signature(signature_path, salt);

    Vector<gf> u = Rainbow::get_result_vector(message_path, salt);

    assert(verify_signature(PK, s, u));
    std::cout << "Successfully verified the Rainbow signature." << std::endl;

    //C(P | I) * (s|u) = 0 -> C*P * s = C*u

    // for efficient verification
    unsigned int k = 3;
    Matrix<gf> C = generate_random_linear_transformation<gf>(k, PK.nrows());
    Matrix<gf> svk = C * PK;
    Vector<gf> u_eff = (Vector<gf>) (C * u);

    assert(verify_signature(svk, s, u_eff));
    std::cout << "Efficiently verified the Rainbow signature." << std::endl;

    // for progressive verification
    unsigned int t = 2;

    assert(progVer(PK, s, u, t));
    std::cout << "Progressively verified the Rainbow signature." << std::endl;

    assert(progVer(svk, s, u_eff, t));
    std::cout << "Efficiently and progressively verified the Rainbow signature." << std::endl << std::endl;


    Vector<gf> signature_guess = Vector<gf>(s.nrows());
    unsigned int efficient_error_count = 0, progressive_error_count = 0, efficient_progressive_error_count = 0;

    for (int i = 0; i < SAMPLE_SIZE; i++) {
        fill_matrix_randomly(signature_guess, 0, Rainbow::q - 1);

        if (verify_signature(svk, signature_guess, u_eff) && !verify_signature(PK, signature_guess, u))
            efficient_error_count++;

        if (progVer(PK, signature_guess, u, t) && !verify_signature(PK, signature_guess, u))
            progressive_error_count++;

        if (progVer(svk, signature_guess, u_eff, t) && !verify_signature(PK, signature_guess, u))
            efficient_progressive_error_count++;
    }

    std::cout << "EFFICIENT VERIFICATION STATS" << std::endl;
    std::cout << "Error percentage: " << (long double) efficient_error_count / SAMPLE_SIZE * 100 << "%" << std::endl;
    std::cout << "Security bits: " << ((efficient_error_count == 0) ? 0: log2((long double) efficient_error_count / SAMPLE_SIZE)) << std::endl << std::endl;

    std::cout << "PROGRESSIVE VERIFICATION STATS" << std::endl;
    std::cout << "Error percentage: " << (long double) progressive_error_count / SAMPLE_SIZE * 100 << "%" << std::endl;
    std::cout << "Security bits: " << ((progressive_error_count == 0) ? 0: log2((long double) progressive_error_count / SAMPLE_SIZE)) << std::endl << std::endl;

    std::cout << "EFFICIENT+PROGRESSIVE VERIFICATION STATS" << std::endl;
    std::cout << "Error percentage: " << (long double) efficient_progressive_error_count / SAMPLE_SIZE * 100 << "%" << std::endl;
    std::cout << "Security bits: " << ((efficient_progressive_error_count == 0) ? 0: log2((long double) efficient_progressive_error_count / SAMPLE_SIZE)) << std::endl << std::endl;

}