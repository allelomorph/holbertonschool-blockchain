/* adapted from blockchain/v0.3/provided/_transaction_print.c */

#include "transaction.h"
/* TAB4 */
#include "hblk_cli.h"
/* printf */
#include <stdio.h>


/**
 * _tx_in_print - used as `action` for llist_for_each to visit each
 *   transaction input in a list, and print its contents
 *
 * @in: pointer to transaction input in list, as iterated through by
 *   llist_for_each
 * @idx: index of `in` in list, as iterated through by llist_for_each
 * @indent: indentation string to use in printing
 *
 * Return: 0 on incremental success (llist_for_each can continue,)
 *   and -2 on failure (-1 reserved for llist_for_each errors)
 */
static int _tx_in_print(tx_in_t const *in, unsigned int idx,
			char const *indent)
{
	(void)idx;

	if (!in)
		return (-2);

	printf("%s" TAB4 TAB4 "{\n", indent);

	printf("%s" TAB4 TAB4 TAB4 "block_hash: ", indent);
	_print_hex_buffer(in->block_hash, sizeof(in->block_hash));
	printf(",\n");
	printf("%s" TAB4 TAB4 TAB4 "tx_id: ", indent);
	_print_hex_buffer(in->tx_id, sizeof(in->tx_id));
	printf(",\n");
	printf("%s" TAB4 TAB4 TAB4 "tx_out_hash: ", indent);
	_print_hex_buffer(in->tx_out_hash, sizeof(in->tx_out_hash));
	printf(",\n");
	printf("%s" TAB4 TAB4 TAB4 "sig: ", indent);
	if (in->sig.len)
		_print_hex_buffer(in->sig.sig, in->sig.len);
	else
		printf("null");
	printf("\n");

	printf("%s" TAB4 TAB4 "}\n", indent);

	return (0);
}


/**
 * _tx_in_print - used as `action` for llist_for_each to visit each
 *   transaction output in a list, and print its contents
 *
 * @out: pointer to transaction output in list, as iterated through by
 *   llist_for_each
 * @idx: index of `out` in list, as iterated through by llist_for_each
 * @indent: indentation string to use in printing
 *
 * Return: 0 on incremental success (llist_for_each can continue,)
 *   and -2 on failure (-1 reserved for llist_for_each errors)
 */
static int _tx_out_print(tx_out_t const *out, unsigned int idx,
			 char const *indent)
{
	(void)idx;
	if (!out)
		return (-2);

	printf("%s" TAB4 TAB4 "{\n", indent);

	printf("%s" TAB4 TAB4 TAB4 "amount: %u,\n", indent, out->amount);
	printf("%s" TAB4 TAB4 TAB4 "pub: ", indent);
	_print_hex_buffer(out->pub, sizeof(out->pub));
	printf(",\n");
	printf("%s" TAB4 TAB4 TAB4 "hash: ", indent);
	_print_hex_buffer(out->hash, sizeof(out->hash));
	printf("\n");

	printf("%s" TAB4 TAB4 "}\n", indent);

	return (0);
}


/**
 * _transaction_print_loop - used as `action` for llist_for_each to
 *   visit each transaction in a list, and print its contents
 *
 * @transaction: pointer to transaction in list, as iterated through by
 *   llist_for_each
 * @idx: index of `transaction` in list, as iterated through by llist_for_each
 * @indent: indentation string to use in printing
 *
 * Return: 0 on incremental success (llist_for_each can continue,)
 *   and -2 on failure (-1 reserved for llist_for_each errors)
 */
int _transaction_print_loop(transaction_t const *transaction,
			    unsigned int idx, char const *indent)
{
	(void)idx;

	if (!transaction)
		return (-2);

	printf("%stransaction: {\n", indent);

	printf("%s" TAB4 "inputs [%u]: [\n", indent,
	       llist_size(transaction->inputs));
	llist_for_each(transaction->inputs, (node_func_t)_tx_in_print,
		(void *)indent);
	printf("%s" TAB4 "],\n", indent);
	printf("%s" TAB4 "outputs [%u]: [\n", indent,
	       llist_size(transaction->outputs));
	llist_for_each(transaction->outputs, (node_func_t)_tx_out_print,
		(void *) indent);
	printf("%s" TAB4 "],\n", indent);
	printf("%s" TAB4 "id: ", indent);
	_print_hex_buffer(transaction->id, sizeof(transaction->id));
	printf("\n");

	printf("%s}\n", indent);

	return (0);
}


/**
 * _transaction_print - Prints the content of a transaction structure
 *
 * @transaction: Pointer to the transaction structure to be printed
 */
void _transaction_print(transaction_t const *transaction)
{
	if (!transaction)
		return;

	printf("Transaction: {\n");

	printf(TAB4 "inputs [%u]: [\n", llist_size(transaction->inputs));
	llist_for_each(transaction->inputs, (node_func_t)_tx_in_print, TAB4);
	printf(TAB4 "],\n");
	printf(TAB4 "outputs [%u]: [\n", llist_size(transaction->outputs));
	llist_for_each(transaction->outputs, (node_func_t)_tx_out_print, TAB4);
	printf(TAB4 "],\n");
	printf(TAB4 "id: ");
	_print_hex_buffer(transaction->id, sizeof(transaction->id));
	printf("\n");

	printf("}\n");
}
