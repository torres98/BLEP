#include <assert.h>
#include <cstring>
#include <iostream>

#include "expected_results.h"
#include "tiny/include/hash_utils.h"

unsigned char message[1000];

int main() {
    
    std::memset(message, 0x41, 1000);


	printf("SHA256 test: ");

    unsigned char digest_sha256[32];

    sha256_raw_str(message, digest_sha256, 1000);
	assert(std::memcmp(digest_sha256, expected_sha256, 32) == 0);

    std::cout << "PASSED" << std::endl;


    std::cout << "SHA512 test: ";

    unsigned char digest_sha512[64];
    
    sha512_raw_str(message, digest_sha512, 1000);
	assert(memcmp(digest_sha512, expected_sha512, 64) == 0);

    std::cout << "PASSED" << std::endl;

    return 0;
}
