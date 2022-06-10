#include <assert.h>

#include "../include/random_utils.h"
#include "../schemes/rainbow.cpp"
#include "../include/standard_verification.h"
#include "../include/efficient_verification.h"
#include "../include/progressive_verification.h"

#define SAMPLE_SIZE 10000

char const *pk_path = "/home/torres/Desktop/Thesis/verification_implementation/tmp/pk.txt";
char const *signature_path = "/home/torres/Desktop/Thesis/verification_implementation/tmp/signature.txt";
char const *message_path = "/home/torres/Desktop/Thesis/verification_implementation/tmp/debug.gdb";

int main(int argc, char *argv[]) {
    
    Rainbow R;
    R = Rainbow(pk_path);

    Vector<gf> v = Rainbow::get_signature_vector(signature_path, message_path);

    assert(verify_signature(R.M, v));
    std::cout << "Successfully verified the Rainbow signature." << std::endl;

    Vector<gf> v_guessed = Vector<gf>(v.nrows());

    // for efficient verification
    unsigned int k = 3;
    Matrix<gf> Z = offVer(R.M, k);

    // for progressive verification
    unsigned int t = 2;

    unsigned int efficient_error_count = 0, progressive_error_count = 0, efficient_progressive_error_count = 0;
    unsigned int effff = 0;

    Vector<gf> u = Vector<gf>(R.M.nrows(), false,  true);
    Vector<gf> u_eff = Vector<gf>(k, false,  true);

    for (int i = 0; i < SAMPLE_SIZE; i++) {
        fill_matrix_randomly(v_guessed, 0, q - 1);

        if (verify_signature(Z, v_guessed) && !verify_signature(R.M, v_guessed))
            efficient_error_count++;

        if (verify_signature(Z, v_guessed, u_eff) && !verify_signature(R.M, v_guessed, u))
            effff++;

        if (progVer(R.M, v_guessed, t) && !verify_signature(R.M, v_guessed))
            progressive_error_count++;

        if (progVer(Z, v_guessed, t) && !verify_signature(R.M, v_guessed))
            efficient_progressive_error_count++;
    }

    std::cout << "EFFICIENT VERIFICATION STATS" << std::endl;
    std::cout << "Error percentage: " << (double) efficient_error_count / SAMPLE_SIZE * 100 << "%" << std::endl;
    std::cout << "Security bits: " << ((efficient_error_count == 0) ? 0: log2((double) efficient_error_count / SAMPLE_SIZE)) << std::endl << std::endl;

    std::cout << "EFFICIENT VERIFICATION STATS" << std::endl;
    std::cout << "Error percentage: " << (double) effff / SAMPLE_SIZE * 100 << "%" << std::endl;
    std::cout << "Security bits: " << ((effff == 0) ? 0: log2((double) effff / SAMPLE_SIZE)) << std::endl << std::endl;

    std::cout << "PROGRESSIVE VERIFICATION STATS" << std::endl;
    std::cout << "Error percentage: " << (double) progressive_error_count / SAMPLE_SIZE * 100 << "%" << std::endl;
    std::cout << "Security bits: " << ((progressive_error_count == 0) ? 0: log2((double) progressive_error_count / SAMPLE_SIZE)) << std::endl << std::endl;

    std::cout << "EFFICIENT+PROGRESSIVE VERIFICATION STATS" << std::endl;
    std::cout << "Error percentage: " << (double) efficient_progressive_error_count / SAMPLE_SIZE * 100 << "%" << std::endl;
    std::cout << "Security bits: " << ((efficient_progressive_error_count == 0) ? 0: log2((double) efficient_progressive_error_count / SAMPLE_SIZE)) << std::endl << std::endl;

}