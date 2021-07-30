/* block_t */
#include "blockchain.h"
/* fprintf */
#include <stdio.h>
/* memcmp */
#include <string.h>


/**
 * validateBlockHashes - verifies hashes of a block in a blockchain and its
 *   previous block
 *
 * @block: pointer to the block to check
 * @prev_block: pointer to the previous block in the blockchain, or NULL if
 *   `block` is the first of the chain
 *
 * Return: 0 on success, or 1 on failure
 */
int validateBlockHashes(block_t const *block, block_t const *prev_block)
{
	uint8_t prev_hash_buf[SHA256_DIGEST_LENGTH],
		hash_buf[SHA256_DIGEST_LENGTH];

	if (!block_hash(prev_block, prev_hash_buf) ||
	    !block_hash(block, hash_buf))
	{
		fprintf(stderr, "block_is_valid: block_hash failure\n");
		return (1);
	}

	if (memcmp(prev_block->hash, prev_hash_buf,
		   SHA256_DIGEST_LENGTH) != 0)
	{
		fprintf(stderr, "block_is_valid: invalid hash of prev_block\n");
		return (1);
	}

	if (memcmp(block->hash, hash_buf,
		   SHA256_DIGEST_LENGTH) != 0)
	{
		fprintf(stderr, "block_is_valid: invalid hash of block\n");
		return (1);
	}

	if (memcmp(block->info.prev_hash, prev_hash_buf,
		   SHA256_DIGEST_LENGTH) != 0)
	{
		fprintf(stderr, "block_is_valid: %s\n",
			"invalid hash of prev_block in block prev_hash");
		return (1);
	}

	return (0);
}


/**
 * block_is_valid - verifies validity of a block in a blockchain
 *
 * @block: pointer to the block to check
 * @prev_block: pointer to the previous block in the blockchain, or NULL if
 *   `block` is the first of the chain
 *
 * Return: 0 on success, or 1 on failure
 */
int block_is_valid(block_t const *block, block_t const *prev_block)
{
	block_t genesis_model = GEN_BLK;

	if (!block)
	{
		fprintf(stderr, "block_is_valid: NULL block\n");
		return (1);
	}

	if (block->info.index == 0)
	{
		if (memcmp(block, &genesis_model, sizeof(block_t)) == 0)
			return (0);

		fprintf(stderr, "block_is_valid: %s\n",
			"block index 0, but not valid Genesis Block");
		return (1);
	}

	if (block->info.index != prev_block->info.index + 1)
	{
		fprintf(stderr, "block_is_valid: %s\n",
			"block index should be prev_block index + 1");
		return (1);
	}

	if (!prev_block)
	{
		fprintf(stderr, "block_is_valid: %s\n",
			"NULL prev_block, but not first block");
		return (1);
	}

	if (block->data.len > BLOCKCHAIN_DATA_MAX)
	{
		fprintf(stderr, "block_is_valid: %s\n",
			"block->data.len over BLOCKCHAIN_DATA_MAX");
		return (1);
	}

	return (validateBlockHashes(block, prev_block));
}
