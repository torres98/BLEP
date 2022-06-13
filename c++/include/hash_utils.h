#ifndef HASH_UTILS_H
#define HASH_UTILS_H

#define SHA_BLOCK_SIZE 32768u

//SHA-256
void sha256_string(const unsigned char *message, unsigned char *output_buffer, unsigned long long mlen);
int sha256_file(const char *file_path, unsigned char *output_buffer);

//SHA-384
void sha384_string(const unsigned char *message, unsigned char *output_buffer, unsigned long long mlen);
int sha384_file(const char *file_path, unsigned char *output_buffer);

//SHA-512
void sha512_string(const unsigned char *message, unsigned char *output_buffer, unsigned long long mlen);
int sha512_file(const char *file_path, unsigned char *output_buffer);

#endif
