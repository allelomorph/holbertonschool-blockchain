#include "transaction.h"
/* fprintf */
#include <stdio.h>


/**
 * transaction_create - creates a transaction
 *
 * @sender: contains the private key of the transaction sender
 * @receiver: contains the public key of the transaction recipient
 * @amount: amount to send
 * @all_unspent: list of all the unspent outputs to date
 *
 * Return: pointer to the created transaction upon success, or NULL upon failure
 */
transaction_t *transaction_create(EC_KEY const *sender, EC_KEY const *receiver,
				  uint32_t amount, llist_t *all_unspent)
{
	if (!sender || !receiver || !all_unspent)
	{
		fprintf(stderr, "transaction_create: NULL parameter(s)\n");
		return (NULL);
	}
}
