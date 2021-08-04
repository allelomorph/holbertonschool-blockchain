/* stdint.h block_t SHA256_DIGEST_LENGTH */
#include "blockchain.h"
/* ULONG_MAX */
#include <limits.h>
/* memcpy */
#include <string.h>


/**
 * block_mine - mines a block in order to insert it in the blockchain
 *
 * @block: block to be mined
 */
void block_mine(block_t *block)
{
	uint64_t i, orig_nonce;
	uint8_t hash_buf[SHA256_DIGEST_LENGTH];

	if (!block)
	{
		fprintf(stderr, "block_mine: NULL parameter\n");
		return;
	}

	orig_nonce = block->info.nonce;
	/* assumes 64-bit architecture */
	for (i = 0; i < ULONG_MAX; i++)
	{
		block->info.nonce = i;

		block_hash(block, hash_buf);
		if (hash_matches_difficulty(hash_buf, block->info.difficulty))
		{
			memcpy(block->hash, hash_buf, SHA256_DIGEST_LENGTH);
			return;
		}
	}

	fprintf(stderr, "block_mine: %s %u\n",
		"no nonce found to produce hash that meets difficulty of",
		block->info.difficulty);
	block->info.nonce = orig_nonce;
}
