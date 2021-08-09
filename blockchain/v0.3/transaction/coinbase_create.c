#include "transaction.h"
/* fprintf */
#include <stdio.h>


/**
 * coinbase_create - creates a coinbase transaction
 *
 * @receiver: contains the public key of the miner, who will receive the
 *   coinbase coins
 * @block_index: index of the block the coinbase transaction will belong to
 *
 * Return: pointer to the created transaction upon success, or NULL upon
 *   failure
 */
transaction_t *coinbase_create(EC_KEY const *receiver, uint32_t block_index)
{
	if (!reciever)
	{
		fprintf(stderr, "coinbase_create: NULL parameter\n");
		return (NULL);
	}
}
