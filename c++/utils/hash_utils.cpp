#include <iostream>

#include "../include/hash_utils.h"
#include "../include/file_utils.h"
#include "openssl/sha.h"

//SHA-256
void sha256_string(const unsigned char *message, unsigned char *output_buffer, unsigned long long mlen) {
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, message, mlen);
    SHA256_Final(output_buffer, &sha256);
}

int sha256_file(const char *file_path, unsigned char *output_buffer) {
    FILE *file = fopen(file_path, "rb");

    if(file == NULL)
        return -534;

    unsigned int file_size = get_file_size(file);

    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    unsigned char *input_buffer = (unsigned char *) malloc(file_size < SHA_BLOCK_SIZE ? file_size: SHA_BLOCK_SIZE);

    if(input_buffer == NULL)
        return ENOMEM;

    unsigned int bytes_read;

    while((bytes_read = fread(input_buffer, 1, SHA_BLOCK_SIZE, file)))
        SHA256_Update(&sha256, input_buffer, bytes_read);

    SHA256_Final(output_buffer, &sha256);

    fclose(file);
    free(input_buffer);

    return 0;
}

//SHA-384
void sha384_string(const unsigned char *message, unsigned char *output_buffer, unsigned long long mlen) {
    SHA512_CTX sha384;
    SHA384_Init(&sha384);
    SHA384_Update(&sha384, message, mlen);
    SHA384_Final(output_buffer, &sha384);
}

int sha384_file(const char *file_path, unsigned char *output_buffer) {
    FILE *file = fopen(file_path, "rb");

    if(file == NULL)
        return -534;

    unsigned int file_size = get_file_size(file);

    SHA512_CTX sha384;
    SHA384_Init(&sha384);
    
    unsigned char *input_buffer = (unsigned char *) malloc(file_size < SHA_BLOCK_SIZE ? file_size: SHA_BLOCK_SIZE);

    if(input_buffer == NULL)
        return ENOMEM;

    unsigned int bytes_read;

    while((bytes_read = fread(input_buffer, 1, SHA_BLOCK_SIZE, file)))
        SHA384_Update(&sha384, input_buffer, bytes_read);

    SHA384_Final(output_buffer, &sha384);

    fclose(file);
    free(input_buffer);

    return 0;
}

//SHA-512
void sha512_string(const unsigned char *message, unsigned char *output_buffer, unsigned long long mlen) {
    SHA512_CTX sha512;
    SHA512_Init(&sha512);
    SHA512_Update(&sha512, message, mlen);
    SHA512_Final(output_buffer, &sha512);
}

int sha512_file(const char *file_path, unsigned char *output_buffer) {
    FILE *file = fopen(file_path, "rb");

    if(file == NULL)
        return -534;

    unsigned int file_size = get_file_size(file);

    SHA512_CTX sha512;
    SHA512_Init(&sha512);
    
    unsigned char *input_buffer = (unsigned char *) malloc(file_size < SHA_BLOCK_SIZE ? file_size: SHA_BLOCK_SIZE);

    if(input_buffer == NULL)
        return ENOMEM;

    unsigned int bytes_read;

    while((bytes_read = fread(input_buffer, 1, SHA_BLOCK_SIZE, file)))
        SHA384_Update(&sha512, input_buffer, bytes_read);

    SHA512_Final(output_buffer, &sha512);

    fclose(file);
    free(input_buffer);

    return 0;
}
