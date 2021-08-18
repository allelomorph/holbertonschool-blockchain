/* blockchain_t */
#include "blockchain.h"
/* llist_destroy */
#include <llist.h>
/* fprintf */
#include <stdio.h>
/* free */
#include <stdlib.h>


/**
 * blockchain_destroy - deletes an existing blockchain, along with all the
 *   blocks it contains
 *
 * @blockchain: pointer to the Blockchain structure to delete
 */
void blockchain_destroy(blockchain_t *blockchain)
{
	if (!blockchain)
	{
		fprintf(stderr, "blockchain_destroy: NULL parameter\n");
		return;
	}

	llist_destroy(blockchain->chain, 1, (node_dtor_t)block_destroy);
	llist_destroy(blockchain->unspent, 1, NULL);

	free(blockchain);
}
