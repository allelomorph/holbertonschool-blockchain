/* -> blockchain.h -> */
/* llist.h stdint.h hblk_crypto.h */
#include "transaction.h"
/* fprintf */
#include <stdio.h>
/* malloc */
#include <stdlib.h>
/* memcpy */
#include <string.h>


/**
 * unspent_tx_out_create - allocates and initializes an unspent transaction
 *   output structure
 *
 * @block_hash: contains the hash of the block where the referenced
 *   transaction output is located
 * @tx_id: contains the hash of a transaction in the block block_hash, where
 *   the referenced transaction output is located
 * @out: points to the referenced transaction output
 *
 * Return: pointer to the created unspent transaction output upon success,
 *   or `NULL` upon failure
 */
unspent_tx_out_t *unspent_tx_out_create(
	uint8_t block_hash[SHA256_DIGEST_LENGTH],
	uint8_t tx_id[SHA256_DIGEST_LENGTH], tx_out_t const *out)
{
	unspent_tx_out_t *new_unspent_tx_out;

	if (!block_hash || !tx_id || !out)
	{
		fprintf(stderr, "unspent_tx_out_create: NULL parameter(s)\n");
		return (NULL);
	}

	new_unspent_tx_out = malloc(sizeof(unspent_tx_out_t));
	if (!new_unspent_tx_out)
	{
		fprintf(stderr, "unspent_tx_out_create: malloc failure\n");
		return (NULL);
	}

	memcpy(&(new_unspent_tx_out->block_hash), block_hash,
	       SHA256_DIGEST_LENGTH);
	memcpy(&(new_unspent_tx_out->tx_id), tx_id,
	       SHA256_DIGEST_LENGTH);
	memcpy(&(new_unspent_tx_out->out), out, sizeof(tx_out_t));

	return (new_unspent_tx_out);
}
