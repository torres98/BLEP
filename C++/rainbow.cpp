#include "utils.cpp"
#include "gf256.cpp"
#include "efficient_verification.cpp"
#include "progressive_verification.cpp"

#define VERSION 1
#define SAMPLE_SIZE 10000

#if VERSION == 1
    static const unsigned short v1 = 36, o1 = 32, o2 = 32, n = 16, DIGEST_SIZE = 32;

    constexpr static void (*hash_str)(unsigned char* message, unsigned char* output_buffer, unsigned long long mlen) = sha256_string;
    constexpr static int (*hash_file)(const char* file_path, unsigned char* output_buffer) = sha256_file;

    typedef gf16 gf;
#elif VERSION == 2
    static const unsigned short v1 = 68, o1 = 32, o2 = 48, n = 256, DIGEST_SIZE = 48;

    constexpr static void (*hash_str)(unsigned char* message, unsigned char* output_buffer, unsigned long long mlen) = sha384_string;
    constexpr static int (*hash_file)(const char* file_path, unsigned char* output_buffer) = sha384_file;

    typedef gf256 gf;
#elif VERSION == 3
    static const unsigned short v1 = 96, o1 = 36, o2 = 64, n = 256, DIGEST_SIZE = 64;

    constexpr static void (*hash_str)(unsigned char* message, unsigned char* output_buffer, unsigned long long mlen) = sha512_string;
    constexpr static int (*hash_file)(const char* file_path, unsigned char* output_buffer) = sha512_file;

    typedef gf256 gf;
#endif

static const unsigned int n_variables = v1+o1+o2, n_polynomials = n_variables - v1, N = n_variables * (n_variables + 1) / 2, element_hex_size = (unsigned int) log2(n) / 4, elements_per_byte = 2 / element_hex_size;
        
class Rainbow {
    private:
        
        static Matrix<gf> extract_public_key(const char *pk_path) {
            Matrix<gf> PK = Matrix<gf>(n_polynomials, N + n_polynomials, true);
            FILE *pk_file = fopen(pk_path, "r");
    
            //consume the header of the pk file
            while (fgetc(pk_file) != '=');
            fgetc(pk_file);

            char element_hex[element_hex_size + 1];

            for (unsigned int i=0; i < N; i++) {
                for (unsigned int j=0; j < n_polynomials; j++) {
                    fgets(element_hex, element_hex_size + 1, pk_file);
                    PK[j][i] = gf(strtol(element_hex, NULL, 16)); 
                }
            }

            fclose(pk_file);

            // concatenate the identity matrix in the right part
            for (unsigned int i=0; i < n_polynomials; i++)
                PK[i][N + i] = gf(1);

            return PK;
        }

    public:
        Matrix<gf> M;

        Rainbow() {}

        Rainbow(const char *pk_path) {
            M = extract_public_key(pk_path);
        }

        static Vector<gf> get_signature_vector(const char* signature_path, const char* message_path) {

            // PARSE SIGNATURE

            Vector<gf> v = Vector<gf>(N + n_polynomials);
            FILE *signature_file = fopen(signature_path, "r");
   
            while (fgetc(signature_file) != '=');
            fgetc(signature_file);

            char element_hex[3];

            for (unsigned int i = 0; i < n_variables; i++) {
                fgets(element_hex, element_hex_size + 1, signature_file);
                v[i] = gf(strtol(element_hex, NULL, 16));
            }

            if (element_hex_size == 1) {
                gf tmp;

                for (unsigned int i = 0; i < n_variables; i+=2) {
                    tmp = v[i+1];
                    v[i+1] = v[i];
                    v[i] = tmp;
                }
            }

            // compute the quadratic products
            unsigned int h = N - 1;

            for (unsigned int i = n_variables; i > 0; i--)
                for (unsigned int j = n_variables; j >= i; j--)
                    v[h--] = v[i-1] * v[j-1];

            
            // PARSE MESSAGE

            // will contain the message digest and the salt
            unsigned char digest[n_polynomials * elements_per_byte];

            // digest = H(m)
            hash_file(message_path, digest);

            // copy the salt at the end of the array (digest = H(m) || salt)
            for (unsigned int i = 0; i < 16; i++) {
                fgets(element_hex, 3, signature_file);
                digest[DIGEST_SIZE + i] = strtol(element_hex, NULL, 16);
            }

            fclose(signature_file);

            // digest = H( H(m) || salt )
            hash_str(digest, digest, DIGEST_SIZE + 16);

            unsigned short remaining_bytes = n_polynomials - DIGEST_SIZE * elements_per_byte;

            if (remaining_bytes != 0) {
                unsigned char additional_digest[DIGEST_SIZE];
                hash_str(digest, additional_digest, DIGEST_SIZE);

                // move the new digest in the last part of the array
                memcpy(digest + DIGEST_SIZE, additional_digest, remaining_bytes);
            }

            // insert the final digest
            if (element_hex_size == 1) {
                for (unsigned int i = 0; i < n_polynomials; i+=2) {
                    v[N + i] = gf(digest[i/2] >> 4);
                    v[N + (i+1)] = gf(digest[i/2] & 0xf);
                }
            } else {
                for (unsigned int i = 0; i < n_polynomials; i++)
                    v[N + i] = gf(digest[i]);
            }

            return v;
        }

};

/*class RainbowI: public Rainbow {
    static const unsigned short v1 = 36, o1 = 32, o2 = 32, element_hex_size = 1, DIGEST_SIZE = 32;
    constexpr static void (*hash)(unsigned char*, unsigned char*, unsigned long long) = sha256_string;
    constexpr static int (*hash_file)(char*, unsigned char*) = sha256_file;
    typedef gf16 gf;

    static const unsigned int n_variables = v1+o1+o2, n_polynomials = n_variables - v1, N = n_variables * (n_variables + 1) / 2;
        
    public:
        using Rainbow::Rainbow;
};

class RainbowIII: public Rainbow {
    static const unsigned short v1 = 68, o1 = 32, o2 = 48, element_hex_size = 2, DIGEST_SIZE = 48;
    constexpr static void (*hash)(unsigned char*, unsigned char*, unsigned long long) = sha384_string;
    constexpr static int (*hash_file)(char*, unsigned char*) = sha384_file;
    typedef gf256 gf;

    static const unsigned int n_variables = v1+o1+o2, n_polynomials = n_variables - v1, N = n_variables * (n_variables + 1) / 2;
        
    public:
        using Rainbow::Rainbow;
};

class RainbowV: public Rainbow {

    static const unsigned short v1 = 96, o1 = 32, o2 = 64, element_hex_size = 2, DIGEST_SIZE = 64;
    //constexpr static void (*hash)(unsigned char*, unsigned char*, unsigned long long) = sha384_string;
    //constexpr static int (*hash_file)(char*, unsigned char*) = sha384_file;
    typedef gf256 gf;

    static const unsigned int n_variables = v1+o1+o2, n_polynomials = n_variables - v1, N = n_variables * (n_variables + 1) / 2;
        
    public:
        using Rainbow::Rainbow;
};*/

char const *pk_path = "/home/torres/Desktop/Thesis/verification_implementation/tmp/pk.txt";
char const *signature_path = "/home/torres/Desktop/Thesis/verification_implementation/tmp/signature.txt";
char const *message_path = "/home/torres/Desktop/Thesis/verification_implementation/tmp/debug.gdb";

int main(int argc, char *argv[]) {
    Rainbow R;
    R = Rainbow(pk_path);

    /*if (argc == 1 || argv[1]) {
        cout << "Defaulting to RainbowI." << endl;
        R = Rainbow(pk_path);
    } else if (argv[1] == "1") {
        R = RainbowI(pk_path);
        Matrix<gf> *v = RainbowI::get_signature_vector(signature_path, message_path);
    } else if (argv[1] == "2") {
        //R = RainbowIII(pk_path);
        typedef gf256 gf;
        //Matrix<gf256> *v = RainbowIII::get_signature_vector(signature_path, message_path);
    } else if (argv[1] == "3") {
        //R = RainbowV(pk_path);
        typedef gf256 gf;
    } else {
        throw exception();
    }*/

    Vector<gf> v = Rainbow::get_signature_vector(signature_path, message_path);

    cout << "VERIFICATION: " << verify_signature(R.M, v) << endl;

    Vector<gf> v_guessed = Vector<gf>(v.nrows());
    gf beta = gf(n / 2);

    // for efficient verification
    unsigned int k = 3;
    Matrix<gf> Z = offVer(R.M, k);

    // for progressive verification
    unsigned int t = 2;

    unsigned int efficient_error_count = 0, progressive_error_count = 0, efficient_progressive_error_count = 0;

    for (int i = 0; i < SAMPLE_SIZE; i++) {
        fill_matrix_randomly(v_guessed, 0, beta);

        if (verify_signature(Z, v_guessed) && !verify_signature(R.M, v_guessed))
            efficient_error_count++;

        if (progVerRand(R.M, v_guessed, t) && !verify_signature(R.M, v_guessed))
            progressive_error_count++;

        if (progVerRand(Z, v_guessed, t) && !verify_signature(R.M, v_guessed))
            efficient_progressive_error_count++;
    }

    cout << "EFFICIENT VERIFICATION STATS" << endl;
    cout << "Error percentage: " << (double) efficient_error_count / SAMPLE_SIZE * 100 << "%" << endl;
    cout << "Security bits: " << ((efficient_error_count == 0) ? 0: log2((double) efficient_error_count / SAMPLE_SIZE)) << endl << endl;

    cout << "PROGRESSIVE VERIFICATION STATS" << endl;
    cout << "Error percentage: " << (double) progressive_error_count / SAMPLE_SIZE * 100 << "%" << endl;
    cout << "Security bits: " << ((progressive_error_count == 0) ? 0: log2((double) progressive_error_count / SAMPLE_SIZE)) << endl << endl;

    cout << "EFFICIENT+PROGRESSIVE VERIFICATION STATS" << endl;
    cout << "Error percentage: " << (double) efficient_progressive_error_count / SAMPLE_SIZE * 100 << "%" << endl;
    cout << "Security bits: " << ((efficient_progressive_error_count == 0) ? 0: log2((double) efficient_progressive_error_count / SAMPLE_SIZE)) << endl << endl;

}