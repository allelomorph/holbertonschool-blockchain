/* adapted from (458) 0x03. Blockchain - Transactions, t14 main_0.c */

#include "blockchain.h"
/* TAB4 */
#include "hblk_cli.h"
/* printf */
#include <stdio.h>


/**
 * _print_unspent - used as `action` for llist_for_each to
 *   visit each UTXO in a list, and print its contents
 *
 * @unspent: pointer to UTXO in list, as iterated through by
 *   llist_for_each
 * @idx: index of `unspent` in list, as iterated through by llist_for_each
 * @indent: indentation string to use in printing
 *
 * Return: 0 on incremental success (llist_for_each can continue,)
 *   and -2 on failure (-1 reserved for llist_for_each errors)
 */
static int _print_unspent(const unspent_tx_out_t *unspent, unsigned int idx,
			  const char *indent)
{
	(void)idx;

	if (!unspent)
		return (-2);

	printf("%s{\n", indent);

	printf("%s" TAB4 "block_hash: ", indent);
	_print_hex_buffer(unspent->block_hash, SHA256_DIGEST_LENGTH);
	printf("\n");

	printf("%s" TAB4 "tx_id: ", indent);
	_print_hex_buffer(unspent->tx_id, SHA256_DIGEST_LENGTH);
	printf("\n");

	printf("%s" TAB4 "amount: %u\n", indent, unspent->out.amount);

	printf("%s" TAB4 "pub: ", indent);
	_print_hex_buffer(unspent->out.pub, EC_PUB_LEN);
	printf("\n");

	printf("%s}\n", indent);

	return (0);
}


/**
 * _print_all_unspent - Print a list of unspent transaction outputs
 *
 * @unspent: List of unspent transaction outputs to print
 * @title: title of unspent list to print before first node
 */
void _print_all_unspent(llist_t *unspent, char *title)
{
	printf("%s [%u]: [\n", title, llist_size(unspent));

	llist_for_each(unspent, (node_func_t)_print_unspent, TAB4);

	printf("]\n");
}
