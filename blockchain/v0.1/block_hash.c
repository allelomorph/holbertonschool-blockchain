#include "blockchain.h"


/**
 * block_hash - computes the hash of a block
 *
 * @block: pointer to the block to be hashed
 * @hash_buf: pointer to buffer for storing hash value
 *
 * Return: pointer to hash_buf, or NULL on failure
 */
uint8_t *block_hash(block_t const *block,
		    uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
}
