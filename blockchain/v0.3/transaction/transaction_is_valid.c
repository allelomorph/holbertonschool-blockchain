#include "transaction.h"
/* fprintf */
#include <stdio.h>


/**
 * transaction_is_valid - checks whether a transaction is valid
 *
 * @transaction: points to the transaction to verify
 * @all_unspent: list of all unspent transaction outputs to date
 *
 * Return: 1 if the transaction is valid, 0 otherwise
 */
int transaction_is_valid(transaction_t const *transaction, llist_t *all_unspent)
{
	if (!transaction || !all_unspent)
	{
		fprintf(stderr, "transaction_is_valid: NULL parameter(s)\n");
		return (0);
	}
}
