/* block_t */
#include "blockchain.h"
/* fprintf */
#include <stdio.h>
/* sha256 */
#include "../../crypto/hblk_crypto.h"


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
	if (!block || !hash_buf)
	{
		fprintf(stderr, "block_hash: NULL parameter(s)\n");
		return (NULL);
	}

	/* hashed data is only block info and the first len bytes of buffer */
	if (sha256((const int8_t *)block,
		   sizeof(block_info_t) + block->data.len, hash_buf) == NULL)
	{
		fprintf(stderr, "block_hash: sha256 failure\n");
		return (NULL);
	}

	return (hash_buf);
}
