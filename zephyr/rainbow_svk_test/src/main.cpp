#include <iostream>
#include <iomanip>

#include <kernel.h>

#include "tiny/include/standard_verification.h"
#include "tiny/include/progressive_verification.h"
#include "tiny/schemes/rainbow.cpp"
#include "tiny/include/matrix.h"
#include "tiny/include/vector.h"
#include "tiny/include/uart_utils.h"

using std::cout;
using std::endl;

using Rainbow::gf;

//maybe default define k and t

unsigned char salt[Rainbow::SALT_SIZE];
unsigned char digest[Rainbow::n_polynomials / Rainbow::elements_per_byte]; //using the same array for the two digest phases


int main() {
    // Variables declaration
    uint32_t start_time, end_time;

    // MESSAGE
    uint32_t mlen = read_uint32(uart_dev); // read message length (4 bytes) 
    Rainbow::get_message_digest(uart_dev, digest, mlen); // read message and compute its digest

    // SIGNATURE
    Vector<gf> s = Rainbow::parse_signature(uart_dev, salt);
    send_ack(uart_dev); //put it back in the rainbow.cpp

    // load the linear transformation and the short verification key as a Matrix objects
    Matrix<gf> SVK = Matrix<gf>(K, Rainbow::N);

    for (unsigned int i = 0; i < SVK.nrows(); i++)
        for (unsigned int j = 0; j < SVK.ncolumns(); j++)
            SVK[i][j] = gf(read_byte(uart_dev));

    send_ack(uart_dev);

    Matrix<gf> C = Matrix<gf>(K, Rainbow::n_polynomials);
    
    for (unsigned int i = 0; i < C.nrows(); i++)
        for (unsigned int j = 0; j < C.ncolumns(); j++)
            C[i][j] = gf(read_byte(uart_dev));

    send_ack(uart_dev);

    // RESULT VECTOR
    // compute final digest
    Rainbow::get_complete_digest(digest, digest, salt);

    cout << std::setfill('0') << std::hex;

    for (unsigned int i = 0; i < Rainbow::n_polynomials / Rainbow::elements_per_byte; i++)
        cout << std::setw(2) << (unsigned int) digest[i];

    cout << endl << std::dec;

    Vector<gf> u_eff = Vector<gf>(C * Rainbow::get_result_vector(digest));
    

    // perform efficient verification
    start_time = k_cycle_get_32();

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

    bool effprog_verification_result = progVer_lazy(SVK, s, u_eff, STEPS);
    
    end_time = k_cycle_get_32() - start_time;

    if (effprog_verification_result) {
        cout << "Success (" << k_cyc_to_us_floor64(end_time) << " microseconds)" << endl;
    } else {
        cout << "Failure (" << k_cyc_to_us_floor64(end_time) << " microseconds)" << endl;
        return -1;
    }

    return 0;

}