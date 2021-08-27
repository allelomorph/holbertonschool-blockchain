/* ->blockchain.h->hblk_crypto.h:EC_key_free */
/* ->blockchain.h->llist.h:llist_destroy */
/* ->blockchain.h->transaction.h:transaction_destroy */
#include "hblk_cli.h"
/* strdup */
#include <string.h>
/* free */
#include <stdlib.h>
/* fprintf perror */
#include <stdio.h>
/* isatty */
#include <unistd.h>


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
	int i;

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
			if (i + 1 >= argc)
			{
				fprintf(stderr,
					"%s: missing parameter for flag '%s'\n",
					argv[0], argv[i]);
				cli_state->exit_code = -2;
				return;
			}
			switch (argv[i][1])
			{
			case 'w':
				i++;
				cli_state->arg_wallet = strdup(argv[i]);
				if (!cli_state->arg_wallet)
				{
					perror("parseArgs: strdup error");
					cli_state->exit_code = -1;
					return;
				}
				break;
			case 'm':
				i++;
				cli_state->arg_mempool = strdup(argv[i]);
				if (!cli_state->arg_mempool)
				{
					perror("parseArgs: strdup error");
					cli_state->exit_code = -1;
					return;
				}
				break;
			case 'b':
				i++;
				cli_state->arg_blockchain = strdup(argv[i]);
				if (!cli_state->arg_blockchain)
				{
					perror("parseArgs: strdup error");
					cli_state->exit_code = -1;
					return;
				}
				break;
			default:
				fprintf(stderr,
					"%s: unsupported flag '%s'\n",
					argv[0], argv[i]);
				cli_state->exit_code = -2;
				return;
			}
		}
		else if (cli_state->arg_script == NULL &&
			 cli_state->arg_script_fd == -1)
			getArgScriptFd(argv[i], cli_state);
	}
}


/**
 * initWlltBlkchnMpl - initializes or loads wallet, mempool, and blockchain
 *   from files
 *
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 */
void initWlltBlkchnMpl(cli_state_t *cli_state)
{
	if (!cli_state)
	{
		fprintf(stderr, "initWlltBlkchnMpl: NULL parameter\n");
		cli_state->exit_code = -1;
		return;
	}

	if (cmd_wallet_load(cli_state->arg_wallet, cli_state) != 0)
	{
		cli_state->wallet = ec_create();
		if (!EC_KEY_check_key(cli_state->wallet))
		{
			printf(TAB4 "Failed to create new wallet\n");
			cli_state->exit_code = -1;
			return;
		}
		printf(TAB4 "Created new wallet for session\n");
	}
/*
	if (!cli_state->arg_blockchain || cmd_load(cli_state->arg_blockchain, cli_state) != 0)
*/
	cli_state->blockchain = blockchain_create();
	if (!cli_state->blockchain)
	{
		printf(TAB4 "Failed to create new blockchain\n");
		cli_state->exit_code = -1;
		return;
	}
	printf(TAB4 "Created new blockchain for session\n");

/*
	if (!cli_state->arg_mempool || cmd_mempool_load(cli_state->arg_mempool, cli_state) != 0)
*/
	cli_state->mempool = llist_create(MT_SUPPORT_FALSE);
	if (!cli_state->mempool)
	{
		printf(TAB4 "Failed to create new empty mempool\n");
		cli_state->exit_code = -1;
		return;
	}
	printf(TAB4 "Created new empty mempool for session\n");
}


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
		initWlltBlkchnMpl(cli_state);

	if (cli_state->exit_code == 0)
	        CLILoop(cli_state);

	/* if not internal failure and in interactive mode */
	if (cli_state->exit_code > -1 && isatty(STDIN_FILENO))
		offerBackupOnExit(cli_state);

	retval = cli_state->exit_code;
	freeCLIState(cli_state);
	return (retval);
}
