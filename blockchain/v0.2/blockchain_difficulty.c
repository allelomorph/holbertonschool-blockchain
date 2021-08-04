#include "blockchain.h"

/**
 * blockchain_difficulty - computes the difficulty to assign to a potential
 *   next block in the blockchain
 *
 * @blockchain: blockchain to analyze
 *
 * Return: difficulty to be assigned to a potential next block in the
 *   blockchain, or 0 on failure
 */
uint32_t blockchain_difficulty(blockchain_t const *blockchain)
{
	if (!blockchain)
	{
		fprintf(stderr, "blockchain_difficulty: NULL parameter\n");
		return 0;
	}
}
