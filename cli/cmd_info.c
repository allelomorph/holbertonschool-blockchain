/* MEMPOOL_PATH_DFLT */
#include "hblk_cli.h"
/* printf fprintf */
#include <stdio.h>
/* memcmp */
#include <string.h>

/*
    Display the number of Blocks in the Blockchain
    Display the number of unspent transaction output
    Display the number of pending transactions in the local transaction pool
*/
#define CMD_INFO_FORMAT "\n" \
	TAB4 "Current status of session blockchain and mempool:\n" \
	TAB4 TAB4 "* blockchain height (block total incl. Genesis Block): %i\n" \
	TAB4 TAB4 "* total unspent transaction outputs (UXTOs): %i\n" \
	TAB4 TAB4 "* total transactions in mempool awaiting confirmation: %i\n" \
	"\n"

#ifdef XXX
/**
 * findAllSenderUnspent - used as `action` for llist_for_each to visit each
 *   unspent output in the blockchain, and add unspent outputs
 *   with a matching public key to a collated list of potential inputs
 *
 * @unspent_tx_out: pointer to unspent output in blockchain->unspent list,
 *   as iterated through by llist_for_each
 * @idx: index of `unspent_tx_out` in blockchain->unspent list, as
 *   iterated through by llist_for_each
 * @su_info: pointer to struct containing all parameters necessary to build a
 *   second list of unspent transactions for a particular public key
 *
 * Return: 0 on incremental success (llist_for_each can continue,)
 *   1 on total success (llist_for_each can end,)
 *   and -2 on failure (-1 reserved for llist_for_each errors)
 */
static int findAllSenderUnspent(unspent_tx_out_t *unspent_tx_out,
			     unsigned int idx, su_info_t *su_info)
{
	(void)idx;

	if (!unspent_tx_out || !su_info)
	{
		fprintf(stderr, "findSenderUnspent: NULL parameter(s)\n");
		return (-2);
	}

	if (memcmp(unspent_tx_out->out.pub, su_info->sender_pub,
		   EC_PUB_LEN) == 0)
	{
		if (llist_add_node(su_info->sender_unspent,
				   unspent_tx_out, ADD_NODE_REAR) != 0)
		{
			fprintf(stderr, "findSenderUnspent: %s\n",
				"llist_add_node failure");
			return (-2);
		}

		su_info->total_unspent_amt += unspent_tx_out->out.amount;
	}

	return (0);
}
#endif

/**
 * cmd_info - displays information about the current blockchain and mempool
 *
 * @arg1: dummy arg to conform to cmd_fp_t typedef
 * @arg2: dummy arg to conform to cmd_fp_t typedef
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 *
 * Return: 0 on success, 1 on failure
 */
int cmd_info(char *arg1, char *arg2, cli_state_t *cli_state)
{
	int block_ct, uxto_ct, mpl_tx_ct;

	(void)arg1;
	(void)arg2;
	if (!cli_state)
	{
		fprintf(stderr, "cmd_info: NULL cli_state parameter\n");
		return (1);
	}

	if (!cli_state->blockchain || !cli_state->blockchain->chain)
	{
		printf(TAB4 "Session blockchain missing (NULL)\n");
	        return (1);
	}

	if (!cli_state->mempool)
	{
		printf(TAB4 "Session mempool missing (NULL)\n");
	        return (1);
	}

	block_ct = llist_size(cli_state->blockchain->chain);
	uxto_ct = llist_size(cli_state->blockchain->unspent);
	mpl_tx_ct = llist_size(cli_state->mempool);

	printf(CMD_INFO_FORMAT, block_ct, uxto_ct, mpl_tx_ct);

	return (0);
}
