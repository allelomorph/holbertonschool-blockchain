/* MEMPOOL_PATH_DFLT */
#include "hblk_cli.h"
/* printf fprintf */
#include <stdio.h>

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

/**
 * cmd_info - displays information about the current blockchain and mempool
 *
 * @arg1: dummy arg to conform to cmd_ref_t.f_ptr typedef
 * @arg2: dummy arg to conform to cmd_ref_t.f_ptr typedef
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
