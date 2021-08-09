#include "transaction.h"
/* fprintf */
#include <stdio.h>


/**
 * tx_in_sign - signs a transaction input, based on the origin transaction id
 *
 * @in: points to the transaction input structure to sign
 * @tx_id: contains the ID (hash) of the transaction the transaction input to
 *   sign is stored in
 * @sender: contains the private key of the receiver of the coins contained in
 *   the transaction output referenced by the transaction input
 * @all_unspent: list of all unspent transaction outputs to date
 *
 * Return: pointer to the resulting signature structure upon success, or NULL
 *   upon failure
 */
sig_t *tx_in_sign(tx_in_t *in, uint8_t const tx_id[SHA256_DIGEST_LENGTH], EC_KEY const *sender, llist_t *all_unspent)
{
	if (!in || !tx_id || !sender || !all_unspent)
	{
		fprintf(stderr, "tx_in_sign: NULL parameter(s)\n");
		return (NULL);
	}
}
