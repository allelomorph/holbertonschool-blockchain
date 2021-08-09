#include "transaction.h"
/* fprintf */
#include <stdio.h>


/**
 * tx_in_create - allocates and initializes a transaction input structure
 *
 * @unspent: points to the unspent transaction output to be converted to a
 *   transaction input
 *
 * Return: pointer to the created transaction input upon success, or NULL upon
 *   failure
 */
tx_in_t *tx_in_create(unspent_tx_out_t const *unspent)
{
	if (!unspent)
	{
		fprintf(stderr, "tx_in_create: NULL parameter\n");
		return (NULL);
	}
}
