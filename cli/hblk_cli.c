/* ->blockchain.h->hblk_crypto.h:EC_key_free */
/* ->blockchain.h->llist.h:llist_destroy */
/* ->blockchain.h->transaction.h:transaction_destroy */
#include "hblk_cli.h"
/* strdup memcpy */
#include <string.h>
/* free */
#include <stdlib.h>
/* fprintf perror */
#include <stdio.h>
/* isatty */
#include <unistd.h>


/**
 * initCLIState - initializes the cli state struct with its default
 *   values and data
 *
 * Return: pointer to new cli state struct, or NULL on failure
 */
cli_state_t *initCLIState(void)
{
	cli_state_t *cli_state;

	cli_state = calloc(1, sizeof(cli_state_t));
	if (!cli_state)
	{
		fprintf(stderr, "initCLIState: calloc failure\n");
		return (NULL);
	}

	/* other state values init to 0 or NULL */
	cli_state->arg_script_fd = -1;
	cli_state->stdin_bup = -1;

	return (cli_state);
}


/**
 * parseArgs - parses argument vector to main to set values in CLI state
 *
 * @argc: `main` argument count
 * @argv: array of `main` argument strings
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 *
 * Return value is indirect, setting cli_state->exit_code to 0 on success,
 *   -1 on internal failure, -2 on invalid args in argv
 */
void parseArgs(int argc, char *argv[], cli_state_t *cli_state)
{
	char *flags = FLAG_ARRAY;
	int i, j, valid_flag = 0;

	if (!argv || !(*argv) || !cli_state)
	{
		fprintf(stderr, "parseArgs: NULL parameter(s)\n");
		cli_state->exit_code = -1;
		return;
	}

	cli_state->exec_name = strdup(argv[0]);
	if (!cli_state->exec_name)
	{
		perror("parseArgs: strdup error");
		cli_state->exit_code = -1;
		return;
	}

	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			for (j = 0; j < FLAG_CT; j++)
			{
				if (argv[i][1] == flags[j])
					valid_flag = 1;
			}
			if (!valid_flag)
			{
				fprintf(stderr,
					"%s: unsupported flag '%s'\n",
					argv[0], argv[i]);
				cli_state->exit_code = -2;
				return;
			}
			switch (argv[i][1])
			{
			case 'w':
				if (!(i + 1 < argc) || argv[i + 1][0] == '-')
				{
					cli_state->arg_wallet =
						strdup(WALLET_DIR_DFLT);
				}
				else
				{
					cli_state->arg_wallet =
						strdup(argv[i + 1]);
					i++;
				}
				break;
			case 'm':
				if (!(i + 1 < argc) || argv[i + 1][0] == '-')
				{
					cli_state->arg_mempool =
						strdup(MEMPOOL_PATH_DFLT);
				}
				else
				{
					cli_state->arg_mempool =
						strdup(argv[i + 1]);
					i++;
				}
				break;
			case 'b':
				if (!(i + 1 < argc) || argv[i + 1][0] == '-')
				{
					cli_state->arg_blockchain =
						strdup(BLKCHN_PATH_DFLT);
				}
				else
				{
					cli_state->arg_blockchain =
						strdup(argv[i + 1]);
					i++;
				}
				break;
			default:
				break;
			}
		}
		else if (cli_state->arg_script == NULL &&
			 cli_state->arg_script_fd == -1)
			getArgScriptFd(argv[i], cli_state);
	}
}


/**
 * copyUnspent - used as `action` for llist_for_each to visit each
 *   unspent output in the blockchain, and copy its nodes into the unspent
 *   output cache
 *
 * @utxo: pointer to unspent output in blockchain->unspent list,
 *   as iterated through by llist_for_each
 * @idx: index of `unspent_tx_out` in blockchain->unspent list, as
 *   iterated through by llist_for_each
 * @unspent_cache: pointer to list of unspent output cache
 *
 * Return: 0 on incremental success (llist_for_each can continue,)
 *   1 on total success (llist_for_each can end,)
 *   and -2 on failure (-1 reserved for llist_for_each errors)
 */
static int copyUnspent(unspent_tx_out_t *utxo,
		       unsigned int idx, llist_t *unspent_cache)
{
	unspent_tx_out_t *utxo_copy;

	(void)idx;

	if (!utxo || !unspent_cache)
	{
		fprintf(stderr, "copyUnspent: NULL parameter(s)\n");
		return (-2);
	}

	utxo_copy = unspent_tx_out_create(utxo->block_hash,
					  utxo->tx_id, &(utxo->out));
	if (!utxo_copy)
	{
		fprintf(stderr, "copyUnspent: unspent_tx_out_create failure\n");
		return (-2);
	}

	if (llist_add_node(unspent_cache, utxo_copy, ADD_NODE_REAR) != 0)
	{
		free(utxo_copy);
		fprintf(stderr, "copyUnspent: %s\n",
			"llist_add_node failure");
		return (-2);
	}

	return (0);
}


/**
 * refreshUnspentCache - sets new unspent transaction output cache to track
 *   pending transactions in the mempool
 *
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 *
 * Return: 0 on success, 1 on failure
 */
int refreshUnspentCache(cli_state_t *cli_state)
{
	if (!cli_state)
	{
		fprintf(stderr, "refreshUnspentCache: NULL parameter\n");
		return (1);
	}

	if (cli_state->unspent_cache)
		llist_destroy(cli_state->unspent_cache, 1, NULL);

	cli_state->unspent_cache = llist_create(MT_SUPPORT_FALSE);
	if (!cli_state->unspent_cache)
	{
		fprintf(stderr, "refreshUnspentCache: llist_create failure\n");
		return (1);
	}

	/* copy blockchain->unspent */
	if (llist_for_each(cli_state->blockchain->unspent,
			   (node_func_t)copyUnspent,
			   cli_state->unspent_cache) != 0)
	{
		llist_destroy(cli_state->unspent_cache, 1, NULL);
		fprintf(stderr, "refreshUnspentCache: llist_for_each failure\n");
		return (1);
	}

	return (0);
}


/**
 * initSession - initializes or loads wallet, mempool, and blockchain
 *   from files, initializes utxo cache
 *
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 */
void initSession(cli_state_t *cli_state)
{
	if (!cli_state)
	{
		fprintf(stderr, "initSession: NULL parameter\n");
		cli_state->exit_code = -1;
		return;
	}

	if (cmd_wallet_load(cli_state->arg_wallet, NULL, cli_state) != 0)
	{
		if (cmd_new_wallet(cli_state) != 0)
		{
			cli_state->exit_code = -1;
			return;
		}
	}

	if (!cli_state->arg_blockchain ||
	    cmd_load(cli_state->arg_blockchain, NULL, cli_state) != 0)
	{
		if (cmd_new_blockchain(cli_state) != 0)
		{
			cli_state->exit_code = -1;
			return;
		}
	}

	if (!cli_state->arg_mempool ||
	    cmd_mempool_load(cli_state->arg_mempool, NULL, cli_state) != 0)
	{
		if (cmd_new_mempool(cli_state) != 0)
		{
			cli_state->exit_code = -1;
			return;
		}
	}

	if (refreshUnspentCache(cli_state) != 0)
		cli_state->exit_code = -1;
}


/**
 * freeCLIState - frees all allocated memory stored in the cli state on repl
 *   exit
 *
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 */
void freeCLIState(cli_state_t *cli_state)
{
	if (cli_state->exec_name)
		free(cli_state->exec_name);

	if (cli_state->arg_wallet)
		free(cli_state->arg_wallet);

	if (cli_state->arg_mempool)
		free(cli_state->arg_mempool);

	if (cli_state->arg_blockchain)
		free(cli_state->arg_blockchain);

	if (cli_state->arg_script)
		free(cli_state->arg_script);

	EC_KEY_free(cli_state->wallet);

	if (cli_state->blockchain)
		blockchain_destroy(cli_state->blockchain);

	llist_destroy(cli_state->mempool, 1, (node_dtor_t)transaction_destroy);

	llist_destroy(cli_state->unspent_cache, 1, NULL);

	free(cli_state);
}


/**
 * main - entry point to hblk_cli
 *
 * @argc: argument count
 * @argv: array of argument strings from command line
 *
 * Return: 0 on success, -1 on failure, -2 if script arg invalid
 */
int main(int argc, char **argv)
{
	cli_state_t *cli_state;
	int retval;

	cli_state = initCLIState();
	if (!cli_state)
		return (-1);

	parseArgs(argc, argv, cli_state);

	if (cli_state->exit_code == 0)
		initSession(cli_state);

	if (cli_state->exit_code == 0)
		CLILoop(cli_state);

	retval = cli_state->exit_code;
	freeCLIState(cli_state);
	return (retval);
}
