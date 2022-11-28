#include <cassert>
#include <cstring>
#include <iostream>

#include "../include/hash_utils.h"

#include "digest_samples.h"

int main() {
    unsigned char message[1000];
    memset(message, 0x41, sizeof(message));


	std::cout << "SHA256 test: ";

    unsigned char digest_sha256[32];
    sha256_string(message, digest_sha256, 1000);

	assert(memcmp(digest_sha256, expected_sha256, 32) == 0);
    std::cout << "PASSED\n";

    std::cout << "SHA512 test: ";

    unsigned char digest_sha384[48];
    sha384_string(message, digest_sha384, 1000);
    
	assert(memcmp(digest_sha384, expected_sha384, 48) == 0);
    std::cout << "PASSED\n";

    std::cout << "SHA512 test: ";

    unsigned char digest_sha512[64];
    sha512_string(message, digest_sha512, 1000);
    
	assert(memcmp(digest_sha512, expected_sha512, 64) == 0);
    std::cout << "PASSED\n";

    return 0;
}
