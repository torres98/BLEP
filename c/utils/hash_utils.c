#include <stdio.h>
#include "openssl/sha.h"

#include "../include/hash_utils.h"
#include "../include/memory_utils.h"
#include "../include/file_utils.h"


#define SHA_BLOCK_SIZE 32768u


void sha256_string(const unsigned char *message, size_t mlen, unsigned char *output_buffer) {
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, message, mlen);
    SHA256_Final(output_buffer, &sha256);
}

void sha256_file(const char *file_path, unsigned char *output_buffer) {
    FILE *file = open_file(file_path, "rb");

    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    size_t file_size = get_file_size(file), bytes_read;
    unsigned char *input_buffer = (unsigned char *) allocate_memory(file_size < SHA_BLOCK_SIZE ? file_size: SHA_BLOCK_SIZE);

    while((bytes_read = fread(input_buffer, 1, SHA_BLOCK_SIZE, file)))
        SHA256_Update(&sha256, input_buffer, bytes_read);

    SHA256_Final(output_buffer, &sha256);

    fclose(file);
    free_memory(input_buffer);
}


void sha384_string(const unsigned char *message, size_t mlen, unsigned char *output_buffer) {
    SHA512_CTX sha384;
    SHA384_Init(&sha384);
    SHA384_Update(&sha384, message, mlen);
    SHA384_Final(output_buffer, &sha384);
}

void sha384_file(const char *file_path, unsigned char *output_buffer) {
    FILE *file = open_file(file_path, "rb");

    SHA512_CTX sha384;
    SHA384_Init(&sha384);
    
    size_t file_size = get_file_size(file), bytes_read;
    unsigned char *input_buffer = (unsigned char *) allocate_memory(file_size < SHA_BLOCK_SIZE ? file_size: SHA_BLOCK_SIZE);

    while((bytes_read = fread(input_buffer, 1, SHA_BLOCK_SIZE, file)))
        SHA384_Update(&sha384, input_buffer, bytes_read);

    SHA384_Final(output_buffer, &sha384);

    fclose(file);
    free_memory(input_buffer);
}


void sha512_string(const unsigned char *message, size_t mlen, unsigned char *output_buffer) {
    SHA512_CTX sha512;
    SHA512_Init(&sha512);
    SHA512_Update(&sha512, message, mlen);
    SHA512_Final(output_buffer, &sha512);
}

void sha512_file(const char *file_path, unsigned char *output_buffer) {
    FILE *file = open_file(file_path, "rb");

    SHA512_CTX sha512;
    SHA512_Init(&sha512);
    
    size_t file_size = get_file_size(file), bytes_read;
    unsigned char *input_buffer = (unsigned char *) allocate_memory(file_size < SHA_BLOCK_SIZE ? file_size: SHA_BLOCK_SIZE);

    while((bytes_read = fread(input_buffer, 1, SHA_BLOCK_SIZE, file)))
        SHA384_Update(&sha512, input_buffer, bytes_read);

    SHA512_Final(output_buffer, &sha512);

    fclose(file);
    free_memory(input_buffer);
}
