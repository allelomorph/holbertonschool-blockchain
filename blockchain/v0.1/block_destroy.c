/* block_t */
#include "blockchain.h"
/* fprintf */
#include <stdio.h>
/* free */
#include <stdlib.h>


/**
 * block_destroy - deletes an existing block
 *
 * @block: pointer to the block to delete
 */
void block_destroy(block_t *block)
{
	if (!block)
	{
		fprintf(stderr, "block_destroy: NULL parameter\n");
		return;
	}

	free(block);
}
