#include <tinycrypt/sha256.h>
#include "SHA/sha384.h"
#include "SHA/sha512.h"

#include "blep/utils/hash.h"
#include "blep/utils/uart.h"

#define CHUNK_SIZE 32768

//we're supposing that each message has not more than 32768 chars
//SHA-256
void sha256_raw_str(const unsigned char *raw_str, unsigned char *output_buffer, unsigned long long mlen) {
    struct tc_sha256_state_struct s;
	tc_sha256_init(&s);
	tc_sha256_update(&s, raw_str, mlen);
	tc_sha256_final(output_buffer, &s);
}

void sha256_from_device(unsigned char *output_buffer, unsigned long long mlen) {
	unsigned char* input_buffer = new unsigned char[mlen < CHUNK_SIZE ? mlen: CHUNK_SIZE];

	struct tc_sha256_state_struct s;
	tc_sha256_init(&s);

	unsigned int nchunks = (mlen >> 15) + 1;
	
	for (unsigned int i = 0; i < nchunks - 1; i++) {   
		read_n_bytes(input_buffer, CHUNK_SIZE);
		tc_sha256_update(&s, input_buffer, CHUNK_SIZE);
	}

	// read last chunk
	read_n_bytes(input_buffer, mlen & (CHUNK_SIZE - 1));
	tc_sha256_update(&s, input_buffer, mlen & (CHUNK_SIZE - 1));

	tc_sha256_final(output_buffer, &s);
	delete[] input_buffer;
}


//SHA-384
void sha384_raw_str(const unsigned char *raw_str, unsigned char *output_buffer, unsigned long long mlen) {
    struct tc_sha384_state_struct s;
	tc_sha384_init(&s);
	tc_sha384_update(&s, raw_str, mlen);
	tc_sha384_final(output_buffer, &s);
}

void sha384_from_device(unsigned char *output_buffer, unsigned long long mlen) {
	unsigned char* input_buffer = new unsigned char[mlen < CHUNK_SIZE ? mlen: CHUNK_SIZE];

	struct tc_sha384_state_struct s;
	tc_sha384_init(&s);

	unsigned int nchunks = (mlen >> 15) + 1;
	
	for (unsigned int i = 0; i < nchunks - 1; i++) {   
		read_n_bytes(input_buffer, CHUNK_SIZE);
		tc_sha384_update(&s, input_buffer, CHUNK_SIZE);
	}

	// read last chunk
	read_n_bytes(input_buffer, mlen & (CHUNK_SIZE - 1));
	tc_sha384_update(&s, input_buffer, mlen & (CHUNK_SIZE - 1));

	tc_sha384_final(output_buffer, &s);
	delete[] input_buffer;
}


//SHA-512
void sha512_raw_str(const unsigned char *raw_str, unsigned char *output_buffer, unsigned long long mlen) {
    struct tc_sha512_state_struct s;
	tc_sha512_init(&s);
	tc_sha512_update(&s, raw_str, mlen);
	tc_sha512_final(output_buffer, &s);
}

void sha512_from_device(unsigned char *output_buffer, unsigned long long mlen) {
	unsigned char* input_buffer = new unsigned char[mlen < CHUNK_SIZE ? mlen: CHUNK_SIZE];

	struct tc_sha512_state_struct s;
	tc_sha512_init(&s);

	unsigned int nchunks = (mlen >> 15) + 1;
	
	for (unsigned int i = 0; i < nchunks - 1; i++) {   
		read_n_bytes(input_buffer, CHUNK_SIZE);
		tc_sha512_update(&s, input_buffer, CHUNK_SIZE);
	}

	// read last chunk
	read_n_bytes(input_buffer, mlen & (CHUNK_SIZE - 1));
	tc_sha512_update(&s, input_buffer, mlen & (CHUNK_SIZE - 1));

	tc_sha512_final(output_buffer, &s);
	delete[] input_buffer;
}
