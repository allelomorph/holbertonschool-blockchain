/* block_t */
#include "blockchain.h"
/* -> blockchain.h -> */
/* llist.h */
#include "transaction.h"
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

	llist_destroy(block->transactions, 1,
			      (node_dtor_t)transaction_destroy);

	free(block);
}
