/* MEMPOOL_PATH_DFLT */
#include "hblk_cli.h"
#include "info_formats.h"
/* printf fprintf */
#include <stdio.h>
/* memcmp */
#include <string.h>


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


/**
 * print_info_wallet - prints information about the current session wallet
 *
 * @full: non-zero if printing as header to `info wallet full`
 * @wallet_unspent: double pointer to store pointer to the list of wallet's
 *   UTXOs; modified by reference
 * @component: non-zero if printing as part of `info`
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 *
 * Return: 0 on success, 1 on failure
 */
int print_info_wallet(int full, llist_t **wallet_unspent,
		      int component, cli_state_t *cli_state)
{
	char *pub_buf;
	su_info_t su_info;
	size_t i, j;

	if (!cli_state || (full && !wallet_unspent))
	{
		fprintf(stderr, "print_info_wallet: NULL parameter\n");
		return (1);
	}

	if (!ec_to_pub(cli_state->wallet, su_info.sender_pub))
		return (1);
	su_info.total_unspent_amt = 0;
	su_info.sender_unspent = llist_create(MT_SUPPORT_FALSE);
	if (!su_info.sender_unspent)
	{
		fprintf(stderr, "print_info_wallet: llist_create failure\n");
		return (1);
	}
	if (llist_for_each(cli_state->unspent_cache,
			   (node_func_t)findAllSenderUnspent, &su_info) != 0)
	{
		fprintf(stderr, "print_info_wallet: llist_for_each failure\n");
		return (1);
	}

	pub_buf = malloc((EC_PUB_LEN * 2) * sizeof(char));
	if (!pub_buf)
	{
		fprintf(stderr, "print_info_wallet: malloc failure\n");
		return (1);
	}
	for (i = 0, j = 0; su_info.sender_pub && i < EC_PUB_LEN; i++, j += 2)
		sprintf(pub_buf + j, "%02x", (su_info.sender_pub)[i]);

	printf(INFO_WALLET_FMT_HDR);
	printf(INFO_WALLET_FMT, pub_buf, llist_size(su_info.sender_unspent),
	       su_info.total_unspent_amt);

	free(pub_buf);
	if (!component)
		printf(INFO_FMT_FTR);
	if (full)
		*wallet_unspent = su_info.sender_unspent;
	else
		llist_destroy(su_info.sender_unspent, 0, NULL);
	return (0);
}


/**
 * print_info_mempool - prints information about the current session mempool
 *
 * @component: non-zero if printing as part of `info`
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 *
 * Return: 0 on success, 1 on failure
 */
int print_info_mempool(int component, cli_state_t *cli_state)
{
	if (!cli_state)
	{
		fprintf(stderr, "print_info_mempool: NULL parameter\n");
		return (1);
	}

	printf(INFO_MEMPOOL_FMT_HDR);
	printf(INFO_MEMPOOL_FMT, llist_size(cli_state->mempool));
	if (!component)
		printf(INFO_FMT_FTR);
	return (0);
}


/**
 * print_info_blockchain - prints information about the current session
 *   blockchain
 *
 * @component: non-zero if printing as part of `info`
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 *
 * Return: 0 on success, 1 on failure
 */
int print_info_blockchain(int component, cli_state_t *cli_state)
{
	if (!cli_state)
	{
		fprintf(stderr, "print_info_blockchain: NULL parameter\n");
		return (1);
	}

	printf(INFO_BLKCHN_FMT_HDR);
	printf(INFO_BLKCHN_FMT,
	       llist_size(cli_state->blockchain->unspent),
	       llist_size(cli_state->unspent_cache),
	       llist_size(cli_state->blockchain->chain));
	if (!component)
		printf(INFO_FMT_FTR);
	return (0);
}


/**
 * print_info - prints information about the current session wallet, mempool,
 *   and blockchain
 *
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 *
 * Return: 0 on success, 1 on failure
 */
int print_info(cli_state_t *cli_state)
{
	if (!cli_state)
	{
		fprintf(stderr, "print_info: NULL parameter\n");
		return (1);
	}

	if (print_info_wallet(0, NULL, 1, cli_state) != 0 ||
	    print_info_mempool(1, cli_state) != 0 ||
	    print_info_blockchain(1, cli_state) != 0)
		return (1);

	printf(INFO_FMT_FTR);
	return (0);
}


/**
 * print_info_wallet_full - prints information about the current session
 *   wallet, including a list of every UTXO
 *
 * @component: non-zero if printing as part of `info full`
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 *
 * Return: 0 on success, 1 on failure
 */
int print_info_wallet_full(int component, cli_state_t *cli_state)
{
	llist_t *wallet_unspent;

	if (!cli_state)
	{
		fprintf(stderr, "print_info_wallet_full: NULL parameter\n");
		return (1);
	}

	if (print_info_wallet(1, &wallet_unspent, 0, cli_state) != 0)
		return (1);
	printf(UNSPENT_CACHE_INTRO);
	_print_all_unspent(wallet_unspent, "Wallet UTXOs");
	if (!component)
		printf(INFO_FMT_FTR);

	llist_destroy(wallet_unspent, 0, NULL);
	return (0);
}


/**
 * print_info_mempool_full - prints information about the current session
 *   mempool, including a list of every transaction
 *
 * @component: non-zero if printing as part of `info full`
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 *
 * Return: 0 on success, 1 on failure
 */
int print_info_mempool_full(int component, cli_state_t *cli_state)
{
	if (!cli_state)
	{
		fprintf(stderr, "print_info_mempool_full: NULL parameter\n");
		return (1);
	}

	if (print_info_mempool(0, cli_state) != 0)
		return (1);

	printf("Mempool transactions [%d]: [\n",
	       llist_size(cli_state->mempool));
	llist_for_each(cli_state->mempool,
		       (node_func_t)_transaction_print_loop,
		       (void *)TAB4);
	printf("]\n");

	if (!component)
		printf(INFO_FMT_FTR);
	return (0);
}


/**
 * print_info_blockchain_full - prints information about the current session
 *   blockchain, including lists of every UTXO and every block
 *
 * @component: non-zero if printing as part of `info full`
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 *
 * Return: 0 on success, 1 on failure
 */
int print_info_blockchain_full(int component, cli_state_t *cli_state)
{
	if (!cli_state)
	{
		fprintf(stderr, "print_info_blockchain_full: NULL parameter\n");
		return (1);
	}

	if (print_info_blockchain(0, cli_state) != 0)
		return (1);
	_print_all_unspent(cli_state->blockchain->unspent,
			   "UTXOs before mempool");
	_print_all_unspent(cli_state->unspent_cache, "UTXOs after mempool:");
	_blockchain_print(cli_state->blockchain);
	if (!component)
		printf(INFO_FMT_FTR);
	return (0);
}


/**
 * print_info_full - prints information about the current session wallet,
 *   mempool, and blockchain, listing out the contents of each
 *
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 *
 * Return: 0 on success, 1 on failure
 */
int print_info_full(cli_state_t *cli_state)
{
	if (!cli_state)
	{
		fprintf(stderr, "print_info_full: NULL parameter\n");
		return (1);
	}

	if (print_info_wallet_full(1, cli_state) != 0 ||
	    print_info_mempool_full(1, cli_state) != 0 ||
	    print_info_blockchain_full(1, cli_state) != 0)
		return (1);

	printf(INFO_FMT_FTR);
	return (0);
}


/**
 * cmd_info - displays information about the current wallet, blockchain and
 *   mempool
 *
 * @arg1: "wallet", "mempool", "blockchain", "full", or NULL, determines what
 *   aspect of the state to display
 * @arg2: "full" or NULL, determines what amount of info to display
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 *
 * Return: 0 on success, 1 on failure
 */
int cmd_info(char *arg1, char *arg2, cli_state_t *cli_state)
{
	char *usage =
		"Usage: info [full]/info [wallet/mempool/blockchain] [full]\n";

	if (!cli_state)
	{
		fprintf(stderr, "cmd_info: NULL cli_state parameter\n");
		return (1);
	}

	if (!arg1)
		return (print_info(cli_state));
	if (strncmp("wallet", arg1, strlen("wallet") + 1) == 0)
	{
		if (!arg2)
			return (print_info_wallet(0, NULL, 0, cli_state));

		if (strncmp("full", arg2, strlen("full") + 1) == 0)
			return (print_info_wallet_full(0, cli_state));

		printf(TAB4 "%s", usage);
		return (1);
	}
	if (strncmp("mempool", arg1, strlen("mempool") + 1) == 0)
	{
		if (!arg2)
			return (print_info_mempool(0, cli_state));

		if (strncmp("full", arg2, strlen("full") + 1) == 0)
			return (print_info_mempool_full(0, cli_state));

		printf(TAB4 "%s", usage);
		return (1);
	}
	if (strncmp("blockchain", arg1, strlen("blockchain") + 1) == 0)
	{
		if (!arg2)
			return (print_info_blockchain(0, cli_state));

		if (strncmp("full", arg2, strlen("full") + 1) == 0)
			return (print_info_blockchain_full(0, cli_state));

		printf(TAB4 "%s", usage);
		return (1);
	}
	if (strncmp("full", arg1, strlen("full") + 1) == 0)
		return (print_info_full(cli_state));

	printf(TAB4 "%s", usage);
	return (1);
}
