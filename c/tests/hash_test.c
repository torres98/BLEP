#include <string.h>
#include <stdio.h>

#include "blep/utils/hash.h"

#include "digest_samples.h"

#define assert(e) if ((e) == 0) return 1

int main() {
    unsigned char message[1000];
    memset(message, 0x41, sizeof(message));


	printf("SHA256 test: ");

    unsigned char digest_sha256[32];
    sha256_string(message, 1000, digest_sha256);

	assert(memcmp(digest_sha256, expected_sha256, 32) == 0);
    printf("PASSED\n");

    printf("SHA512 test: ");

    unsigned char digest_sha384[48];
    sha384_string(message, 1000, digest_sha384);
    
	assert(memcmp(digest_sha384, expected_sha384, 48) == 0);
    printf("PASSED\n");

    printf("SHA512 test: ");

    unsigned char digest_sha512[64];
    sha512_string(message, 1000, digest_sha512);
    
	assert(memcmp(digest_sha512, expected_sha512, 64) == 0);
    printf("PASSED\n");

    return 0;
}
