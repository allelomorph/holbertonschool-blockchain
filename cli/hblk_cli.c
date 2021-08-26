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


/**
 * initCLIState - initializes the cli state struct with its default
 *   values and data
 *
 * @exec_name: name of script from argv[1] of main, if used
 *
 * Return: pointer to new cli state struct, or NULL on failure
 */
cli_state_t *initCLIState(char *exec_name)
{
	cli_state_t *cli_state;

	if (!exec_name)
	{
		fprintf(stderr, "initCLIState: NULL parameter\n");
		return (NULL);
	}
	cli_state = malloc(sizeof(cli_state_t));
	if (!cli_state)
	{
		fprintf(stderr, "initCLIState: malloc failure\n");
		return (NULL);
	}

        cli_state->exec_name = strdup(exec_name);
	if (!cli_state->exec_name)
	{
		perror("initCLIState: strdup error");
		return (NULL);
	}
	cli_state->arg_script_path = NULL;
	cli_state->arg_script_fd = -1;
	cli_state->stdin_bup = -1;
	cli_state->loop_ct = 0;
	cli_state->exit_code = 0;
	cli_state->user_wallet = NULL; /* dummy init */
/*
	if (!cmd_wallet_load(cli_state))
	{
		fprintf(stderr, "initCLIState: cmd_wallet_load failure\n");
		return (NULL);
	}
*/
	cli_state->blockchain = blockchain_create();
	if (!cli_state->blockchain)
	{
		fprintf(stderr, "initCLIState: blockchain_create failure\n");
		return (NULL);
	}
	cli_state->mempool = llist_create(MT_SUPPORT_FALSE);
	if (!cli_state->mempool)
	{
		fprintf(stderr, "initCLIState: llist_create failure\n");
		return (NULL);
	}

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

	if (cli_state->arg_script_path)
		free(cli_state->arg_script_path);

	EC_KEY_free(cli_state->user_wallet);

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

        cli_state = initCLIState(argv[0]);
	if (!cli_state)
		return (-1);

        /* getInitScriptFd(cli_state); */

	if (argc > 1) /* ignore args after first */
	        getArgScriptFd(argv[1], cli_state);

	/* script read failures set exit code to -2 */
	if (cli_state->exit_code == 0)
	        CLILoop(cli_state);

	retval = cli_state->exit_code;
	freeCLIState(cli_state);

	return (retval);
}
