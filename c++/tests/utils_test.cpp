#include <assert.h>

#include "../utils.cpp"

int main() {
    unsigned char message[1000];
    memset(message, 0x41, sizeof(message));

	std::cout << "SHA256 test: ";

    unsigned char digest_sha256[32];
	const uint8_t expected_sha256[32] = {
		0xc2, 0xe6, 0x86, 0x82, 0x34, 0x89, 0xce, 0xd2,
        0x01, 0x7f, 0x60, 0x59, 0xb8, 0xb2, 0x39, 0x31,
        0x8b, 0x63, 0x64, 0xf6, 0xdc, 0xd8, 0x35, 0xd0,
		0xa5, 0x19, 0x10, 0x5a, 0x1e, 0xad, 0xd6, 0xe4
	};

    sha256_string(message, digest_sha256, 1000);

	assert(memcmp(digest_sha256, expected_sha256, 32) == 0);
    std::cout << "PASSED\n";


    std::cout << "SHA512 test: ";

    unsigned char digest_sha512[64];
    const unsigned char expected_sha512[64] = {
        0x32, 0x9c, 0x52, 0xac, 0x62, 0xd1, 0xfe, 0x73,
        0x11, 0x51, 0xf2, 0xb8, 0x95, 0xa0, 0x04, 0x75,
        0x44, 0x5e, 0xf7, 0x4f, 0x50, 0xb9, 0x79, 0xc6,
        0xf7, 0xbb, 0x7c, 0xae, 0x34, 0x93, 0x28, 0xc1,
        0xd4, 0xcb, 0x4f, 0x72, 0x61, 0xa0, 0xab, 0x43,
        0xf9, 0x36, 0xa2, 0x4b, 0x00, 0x06, 0x51, 0xd4,
        0xa8, 0x24, 0xfc, 0xdd, 0x57, 0x7f, 0x21, 0x1a,
        0xef, 0x8f, 0x80, 0x6b, 0x16, 0xaf, 0xe8, 0xaf
    };
    
    sha512_string(message, digest_sha512, 1000);
    
	assert(memcmp(digest_sha512, expected_sha512, 64) == 0);
    std::cout << "PASSED\n";

    return 0;
}