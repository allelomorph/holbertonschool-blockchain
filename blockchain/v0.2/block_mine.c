#include "blockchain.h"

/**
 * block_mine - mines a block in order to insert it in the blockchain
 *
 * @block: block to be mined
 */
void block_mine(block_t *block)
{
	if (!block)
	{
		fprintf(stderr, "block_mine: NULL parameter\n");
		return;
	}
}
