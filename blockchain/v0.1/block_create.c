#include "blockchain.h"


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
}
