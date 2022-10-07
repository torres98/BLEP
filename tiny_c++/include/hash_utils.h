#ifndef HASH_UTILS_H
#define HASH_UTILS_H

#include <zephyr/device.h>


void sha256_raw_str(unsigned char *raw_str, unsigned char *output_buffer, unsigned long long mlen);
void sha256_from_device(unsigned char *output_buffer, unsigned long long mlen);

void sha384_raw_str(unsigned char *raw_str, unsigned char *output_buffer, unsigned long long mlen);
void sha384_from_device(unsigned char *output_buffer, unsigned long long mlen);
	
void sha512_raw_str(unsigned char *raw_str, unsigned char *output_buffer, unsigned long long mlen);
void sha512_from_device(unsigned char *output_buffer, unsigned long long mlen);

#endif
