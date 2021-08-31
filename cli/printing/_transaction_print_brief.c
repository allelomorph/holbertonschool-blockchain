/* adapted from blockchain/v0.3/provided/_transaction_print_brief.c */

#include "transaction.h"
/* TAB4 */
#include "hblk_cli.h"
/* printf */
#include <stdio.h>


/**
 * _transaction_print_brief_loop - used as `action` for llist_for_each to
 *   visit each transaction in a list, print its contents
 *
 * @transaction: pointer to transaction in list, as iterated through by
 *   llist_for_each
 * @idx: index of `transaction` in list, as iterated through by llist_for_each
 * @indent: indentation string to use in printing
 *
 * Return: 0 on incremental success (llist_for_each can continue,)
 *   and -2 on failure (-1 reserved for llist_for_each errors)
 */
int _transaction_print_brief_loop(transaction_t const *transaction,
					  unsigned int idx, char const *indent)
{
	tx_out_t const *out;

	(void)idx;
	if (!transaction)
		return (-2);

	out = llist_get_head(transaction->outputs);

	printf("%sTransaction: {\n", indent);

	printf("%s" TAB4 "amount: %u from %d inputs,\n", indent, out->amount,
		llist_size(transaction->inputs));
	printf("%s" TAB4 "receiver: ", indent);
	_print_hex_buffer(out->pub, EC_PUB_LEN);
	printf("\n");
	printf("%s" TAB4 "id: ", indent);
	_print_hex_buffer(transaction->id, sizeof(transaction->id));
	printf("\n");

	printf("%s}\n", indent);

	return (0);
}

/**
 * _transaction_print_brief - Prints the content of a transaction structure
 *
 * @transaction: Pointer to the transaction structure to be printed
 */
void _transaction_print_brief(transaction_t const *transaction)
{
	tx_out_t const *out;

	if (!transaction)
		return;

	out = llist_get_head(transaction->outputs);

	printf("Transaction: {\n");

	printf(TAB4 "amount: %u from %d inputs,\n", out->amount,
		llist_size(transaction->inputs));
	printf(TAB4 "receiver: ");
	_print_hex_buffer(out->pub, EC_PUB_LEN);
	printf("\n");
	printf(TAB4 "id: ");
	_print_hex_buffer(transaction->id, sizeof(transaction->id));
	printf("\n");

	printf("}\n");
}
