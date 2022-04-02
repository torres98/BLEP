#include <random>
#include <cstring> //for memcpy
#include <algorithm> //for shuffle
#include <fstream> //for ifstream
#include <sstream> //for stringstream
#include <iostream>

#include "matrix.cpp"
#include "openssl/sha.h"

#define string std::string
#define endl std::endl
#define cout std::cout

#ifndef _UTILS
#define _UTILS

/**************/
// FILE UTILS //
/**************/

string read(string file_path) {
    
    std::ifstream file(file_path);
    std::stringstream file_content;

    if (file.fail()) {
        cout << "Error during read of " << file_path << endl;
        exit(-1);
    }

    file_content << file.rdbuf();

    file.close();
    
    return file_content.str();
}

/****************/
// RANDOM UTILS //
/****************/

std::random_device rd;
static std::mt19937 rng(rd());

template <typename Element>
void fill_matrix_randomly(Matrix<Element> &M, uint8_t min = 0, uint8_t max = 255, uint32_t seed = 0) {

    if (seed != 0)  
        rng.seed(seed);

    std::uniform_int_distribution<uint8_t> element_dist(min, max);
    
    for (unsigned int i = 0; i < M.rows; i++)
        for (unsigned int j = 0; j < M.columns; j++)
            M[i][j] = Element(element_dist(rng));

}

template <typename Element>
void shuffle_array(Element* arr, unsigned int n) {
    shuffle(arr, arr + n, rng);
}

/**************/
// HASH UTILS //
/**************/

//SHA-256
void sha256_string(unsigned char *message, unsigned char *output_buffer, unsigned long long mlen) {
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, message, mlen);
    SHA256_Final(output_buffer, &sha256);
}

int sha256_file(char *file_path, unsigned char *output_buffer) {
    FILE *file = fopen(file_path, "rb");
    if(!file) return -534;

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    const int bufSize = 32768;
    unsigned char *buffer = (unsigned char *) malloc(bufSize);
    int bytesRead = 0;

    if(!buffer) return ENOMEM;

    while((bytesRead = fread(buffer, 1, bufSize, file))) {
        SHA256_Update(&sha256, buffer, bytesRead);
    }

    SHA256_Final(output_buffer, &sha256);

    fclose(file);
    free(buffer);
    return 0;
}

//SHA-384
void sha384_string(unsigned char *message, unsigned char *output_buffer, unsigned long long mlen) {
    SHA512_CTX sha384;
    SHA384_Init(&sha384);
    SHA384_Update(&sha384, message, mlen);
    SHA384_Final(output_buffer, &sha384);
}

int sha384_file(char *file_path, unsigned char *output_buffer) {
    FILE *file = fopen(file_path, "rb");
    if(!file) return -534;

    SHA512_CTX sha384;
    SHA384_Init(&sha384);
    const int bufSize = 32768;
    unsigned char *buffer = (unsigned char *) malloc(bufSize);
    int bytesRead = 0;

    if(!buffer) return ENOMEM;

    while((bytesRead = fread(buffer, 1, bufSize, file))) {
        SHA384_Update(&sha384, buffer, bytesRead);
    }

    SHA384_Final(output_buffer, &sha384);

    fclose(file);
    free(buffer);
    return 0;
}

//SHA-512
void sha512_string(unsigned char *message, unsigned char *output_buffer, unsigned long long mlen) {
    SHA512_CTX sha512;
    SHA512_Init(&sha512);
    SHA512_Update(&sha512, message, mlen);
    SHA512_Final(output_buffer, &sha512);
}

int sha512_file(char *file_path, unsigned char *output_buffer) {
    FILE *file = fopen(file_path, "rb");
    if(!file) return -534;

    SHA512_CTX sha512;
    SHA512_Init(&sha512);
    const int bufSize = 32768;
    unsigned char *buffer = (unsigned char *) malloc(bufSize);
    int bytesRead = 0;

    if(!buffer) return ENOMEM;

    while((bytesRead = fread(buffer, 1, bufSize, file))) {
        SHA512_Update(&sha512, buffer, bytesRead);
    }

    SHA384_Final(output_buffer, &sha512);

    fclose(file);
    free(buffer);
    return 0;
}

#endif