#ifndef __TC_SHA384_H__
#define __TC_SHA384_H__

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TC_SHA384_BLOCK_SIZE (128)
#define TC_SHA384_DIGEST_SIZE (48)
#define TC_SHA384_STATE_BLOCKS (TC_SHA384_DIGEST_SIZE/6)

struct tc_sha384_state_struct {
	uint64_t iv[TC_SHA384_STATE_BLOCKS];
	uint64_t bits_hashed;
	uint8_t leftover[TC_SHA384_BLOCK_SIZE];
	size_t leftover_offset;
};

typedef struct tc_sha384_state_struct *TCSha384State_t;

/**
 *  @brief SHA384 initialization procedure
 *  Initializes s
 *  @return returns TC_CRYPTO_SUCCESS (1)
 *          returns TC_CRYPTO_FAIL (0) if s == NULL
 *  @param s Sha512 state struct
 */
int tc_sha384_init(TCSha384State_t s);

/**
 *  @brief SHA384 update procedure
 *  Hashes data_length bytes addressed by data into state s
 *  @return returns TC_CRYPTO_SUCCESS (1)
 *          returns TC_CRYPTO_FAIL (0) if:
 *                s == NULL,
 *                s->iv == NULL,
 *                data == NULL
 *  @note Assumes s has been initialized by tc_sha384_init
 *  @warning The state buffer 'leftover' is left in memory after processing
 *           If your application intends to have sensitive data in this
 *           buffer, remind to erase it after the data has been processed
 *  @param s Sha512 state struct
 *  @param data message to hash
 *  @param datalen length of message to hash
 */
int tc_sha384_update(TCSha384State_t s, const uint8_t *data, size_t datalen);

/**
 *  @brief SHA384 final procedure
 *  Inserts the completed hash computation into digest
 *  @return returns TC_CRYPTO_SUCCESS (1)
 *          returns TC_CRYPTO_FAIL (0) if:
 *                s == NULL,
 *                s->iv == NULL,
 *                digest == NULL
 *  @note Assumes: s has been initialized by tc_sha384_init
 *        digest points to at least TC_SHA384_DIGEST_SIZE bytes
 *  @warning The state buffer 'leftover' is left in memory after processing
 *           If your application intends to have sensitive data in this
 *           buffer, remind to erase it after the data has been processed
 *  @param digest unsigned eight bit integer
 *  @param Sha512 state struct
 */
int tc_sha384_final(uint8_t *digest, TCSha384State_t s);


#ifdef __cplusplus
}
#endif

#endif /* __TC_SHA384_H__ */
