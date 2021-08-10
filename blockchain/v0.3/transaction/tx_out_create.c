/* -> blockchain.h -> */
/* llist.h stdint.h hblk_crypto.h */
#include "transaction.h"
/* fprintf */
#include <stdio.h>
/* malloc free */
#include <stdlib.h>
/* memcpy */
#include <string.h>


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
	tx_out_t *new_tx_out;

	if (!pub)
	{
		fprintf(stderr, "tx_out_create: NULL parameter\n");
		return (NULL);
	}

	new_tx_out = malloc(sizeof(tx_out_t));
	if (!new_tx_out)
	{
		fprintf(stderr, "tx_out_create: malloc failure\n");
		return (NULL);
	}

	new_tx_out->amount = amount;
	memcpy(new_tx_out->pub, pub, EC_PUB_LEN);

	if (!sha256((const int8_t *)new_tx_out, sizeof(uint32_t) + EC_PUB_LEN,
		   new_tx_out->hash))
	{
		fprintf(stderr, "tx_out_create: sha256 failure\n");
		free(new_tx_out);
		return (NULL);
	}

	return (new_tx_out);
}
