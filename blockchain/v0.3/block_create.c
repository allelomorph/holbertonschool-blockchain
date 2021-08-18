/* block_t */
#include "blockchain.h"
/* fprintf */
#include <stdio.h>
/* calloc free */
#include <stdlib.h>
/* time */
#include <time.h>
/* memcpy */
#include <string.h>


/**
 * block_create - creates and initializes a block structure
 *
 * @prev: pointer to the previous block in the blockchain
 * @data: pointer to a memory area to duplicate in the block’s data
 * @data_len: number of bytes to duplicate in data (if greater than
 *   BLOCKCHAIN_DATA_MAX, then only BLOCKCHAIN_DATA_MAX bytes are duplicated)
 *
 * Return: newly allocated block, or NULL on failure
 */
block_t *block_create(block_t const *prev, int8_t const *data,
		      uint32_t data_len)
{
	block_t *new_blk;

	/* prev should never be NULL as "empty" chain has a Genesis Block */
	if (!prev || !data)
	{
		fprintf(stderr, "block_create: NULL parameter(s)\n");
		return (NULL);
	}

	new_blk = calloc(1, sizeof(block_t));
	if (!new_blk)
	{
		fprintf(stderr, "block_create: calloc failure\n");
		return (NULL);
	}

	new_blk->info.index = prev->info.index + 1;
	new_blk->info.timestamp = (uint64_t)time(NULL);
	memcpy(new_blk->info.prev_hash, prev->hash, SHA256_DIGEST_LENGTH);

	new_blk->data.len = data_len < BLOCKCHAIN_DATA_MAX ?
		data_len : BLOCKCHAIN_DATA_MAX;
	memcpy(new_blk->data.buffer, data, new_blk->data.len);

	new_blk->transactions = llist_create(MT_SUPPORT_FALSE);
	if (!new_blk->transactions)
	{
		fprintf(stderr, "block_create: llist_create failure\n");
		free(new_blk);
		return (NULL);
	}

	return (new_blk);
}
