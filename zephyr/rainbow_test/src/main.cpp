#include <iostream>
#include <iomanip>

#include <kernel.h>

#include "tiny/include/standard_verification.h"
#include "tiny/include/progressive_verification.h"
#include "tiny/schemes/rainbow.cpp"
#include "tiny/include/matrix.h"
#include "tiny/include/vector.h"
#include "tiny/include/uart_utils.h"

#include "svk.h"

using std::cout;
using std::endl;

using Rainbow::gf;

unsigned char salt[Rainbow::SALT_SIZE];
unsigned char final_digest[Rainbow::n_polynomials / Rainbow::elements_per_byte];

int main() {
    // Variables declaration
    uint32_t start_time, end_time;

    //read signature
    Vector<gf> s = Rainbow::parse_signature(uart_dev, salt);
    send_ack(uart_dev); //put it back in the rainbow.cpp

    //send back what you calculated
    /*cout << std::setfill('0') << std::hex;

    for (unsigned int i = 0; i < Rainbow::N; i++)
        cout << std::setw(Rainbow::element_hex_size) << s[i];
            
    cout << endl << std::dec;*/

    // read message length (4 bytes) 
    uint32_t mlen = read_uint32(uart_dev);

    // read message and compute its Rainbow digest
    Rainbow::get_complete_digest(uart_dev, final_digest, salt, mlen);

    cout << std::setfill('0') << std::hex;

    for (unsigned int i = 0; i < Rainbow::n_polynomials / Rainbow::elements_per_byte; i++)
        cout << std::setw(2) << (unsigned int) final_digest[i];

    cout << endl << std::dec;

    // load the linear transformation and the short verification key as a Matrix objects
    Matrix<gf> SVK = Matrix<gf>((gf*) short_private_key, k, Rainbow::N);
    Matrix<gf> C = Matrix<gf>((gf*) private_transformation, k, Rainbow::n_polynomials);
    
    // perform efficient verification
    start_time = k_cycle_get_32();

    Vector<gf> u_eff = Vector<gf>(C * Rainbow::get_result_vector(final_digest));
    
    bool eff_verification_result = verify_signature_lazy(SVK, s, u_eff);
    
    end_time = k_cycle_get_32() - start_time;

    if (eff_verification_result) {
        cout << "Success (" << k_cyc_to_us_floor64(end_time) << " microseconds)" << endl;
    } else {
        cout << "Failure (" << k_cyc_to_us_floor64(end_time) << " microseconds)" << endl;
        return -1;
    }

    // perform efficient + progressive verification
    start_time = k_cycle_get_32();
    unsigned int t = 1;

    bool effprog_verification_result = progVer_lazy(SVK, s, u_eff, t);
    
    end_time = k_cycle_get_32() - start_time;

    if (effprog_verification_result) {
        cout << "Success (" << k_cyc_to_us_floor64(end_time) << " microseconds)" << endl;
    } else {
        cout << "Failure (" << k_cyc_to_us_floor64(end_time) << " microseconds)" << endl;
        return -1;
    }

    return 0;

}