#include <cstring>
#include <iostream>

#include "blep/utils/hash.h"

#include "digest_samples.h"

#define assert(e) if ((e) == 0) { std::cout << "FAILED" << std::endl; return 1; }

int main() {
    unsigned char message[1000];
    memset(message, 0x41, sizeof(message));


	std::cout << "SHA256 test: ";

    unsigned char digest_sha256[32];
    sha256_string(message, 1000, digest_sha256);

	assert(memcmp(digest_sha256, expected_sha256, 32) == 0);
    std::cout << "PASSED" << std::endl;

    std::cout << "SHA512 test: ";

    unsigned char digest_sha384[48];
    sha384_string(message, 1000, digest_sha384);
    
	assert(memcmp(digest_sha384, expected_sha384, 48) == 0);
    std::cout << "PASSED" << std::endl;

    std::cout << "SHA512 test: ";

    unsigned char digest_sha512[64];
    sha512_string(message, 1000, digest_sha512);
    
	assert(memcmp(digest_sha512, expected_sha512, 64) == 0);
    std::cout << "PASSED" << std::endl;

    return 0;
}
