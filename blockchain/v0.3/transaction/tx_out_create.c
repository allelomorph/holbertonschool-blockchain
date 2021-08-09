#include "transaction.h"
/* fprintf */
#include <stdio.h>


/**
 * tx_out_create - allocates and initializes a transaction output structure
 *
 * @amount: the amount of the transaction
 * @pub: the public key of the transaction recipient
 *
 * Return: pointer to the created transaction output upon success, or NULL
 *   upon failure
 */
tx_out_t *tx_out_create(uint32_t amount, uint8_t const pub[EC_PUB_LEN])
{
	if (!pub)
	{
		fprintf(stderr, "tx_out_create: NULL parameter\n");
		return (NULL);
	}
}
