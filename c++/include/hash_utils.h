#ifndef HASH_UTILS_H
#define HASH_UTILS_H

#define SHA_BLOCK_SIZE 32768u


void sha256_string(const unsigned char *message, size_t mlen, unsigned char *output_buffer);
void sha256_file(const char *file_path, unsigned char *output_buffer);

void sha384_string(const unsigned char *message, size_t mlen, unsigned char *output_buffer);
void sha384_file(const char *file_path, unsigned char *output_buffer);

void sha512_string(const unsigned char *message, size_t mlen, unsigned char *output_buffer);
void sha512_file(const char *file_path, unsigned char *output_buffer);

#endif
