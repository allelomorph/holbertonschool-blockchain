/* blockchain_t */
#include "blockchain.h"
/* llist_errno llist_destroy */
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

	/* could also potentially cast block_destroy to node_dtor_t */
	if (llist_destroy(blockchain->chain, 1, NULL) != 0)
	{
		fprintf(stderr, "blockchain_destroy: llist_destroy: %s\n",
			strE_LLIST(llist_errno));
		return;
	}

	free(blockchain);
}
