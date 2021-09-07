/*  */
#include "hblk_cli.h"
/* printf fprintf */
#include <stdio.h>
/* strncmp strlen */
#include <string.h>


/**
 * cmd_new_wallet - replaces session wallet with a freshly created alternative
 *
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 *
 * Return: 0 on success, 1 on failure
 */
int cmd_new_wallet(cli_state_t *cli_state)
{
	EC_KEY *new_wallet;

	if (!cli_state)
	{
		fprintf(stderr, "cmd_new_wallet: NULL cli_state parameter\n");
		return (1);
	}

	new_wallet = ec_create();
	if (!EC_KEY_check_key(new_wallet))
	{
		if (new_wallet)
			EC_KEY_free(new_wallet);

		printf(TAB4 "Failed to create new wallet\n");
		return (1);
	}

	if (cli_state->wallet)
		EC_KEY_free(cli_state->wallet);
	cli_state->wallet = new_wallet;
	printf(TAB4 "Created new wallet for session\n");
	return (0);
}


/**
 * cmd_new_blockchain - replaces session blockchain with a freshly created
 *   alternative
 *
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 *
 * Return: 0 on success, 1 on failure
 */
int cmd_new_blockchain(cli_state_t *cli_state)
{
	blockchain_t *new_blockchain;

	if (!cli_state)
	{
		fprintf(stderr, "cmd_new_blockchain: NULL cli_state parameter\n");
		return (1);
	}

	new_blockchain = blockchain_create();
	if (!new_blockchain)
	{
		printf(TAB4 "Failed to create new blockchain\n");
		return (1);
	}

	if (cli_state->blockchain)
		blockchain_destroy(cli_state->blockchain);
	cli_state->blockchain = new_blockchain;
	printf(TAB4 "Created new blockchain for session\n");

	if (refreshUnspentCache(cli_state) != 0)
		return (1);

	return (0);
}


/**
 * cmd_new_mempool - replaces mempool with a freshly created alternative
 *
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 *
 * Return: 0 on success, 1 on failure
 */
int cmd_new_mempool(cli_state_t *cli_state)
{
	llist_t *new_mempool;

	if (!cli_state)
	{
		fprintf(stderr, "cmd_new_mempool: NULL cli_state parameter\n");
		return (1);
	}

	new_mempool = llist_create(MT_SUPPORT_FALSE);
	if (!new_mempool)
	{
		printf(TAB4 "Failed to create new empty mempool\n");
		return (1);
	}

	if (cli_state->mempool)
	{
		llist_destroy(cli_state->mempool, 1,
			      (node_dtor_t)transaction_destroy);
	}
	cli_state->mempool = new_mempool;
	printf(TAB4 "Created new empty mempool for session\n");

	if (refreshUnspentCache(cli_state) != 0)
		return (1);

	return (0);
}


/**
 * cmd_new - replaces session wallet, blockchain or mempool with a freshly
 *   created alternative
 *
 * @aspect: name of session data to refresh
 * @arg2: dummy arg to conform to cmd_fp_t typedef
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 *
 * Return: 0 on success, 1 on failure
 */
int cmd_new(char *aspect, char *arg2, cli_state_t *cli_state)
{
	(void)arg2;
	if (!cli_state)
	{
		fprintf(stderr, "cmd_new: NULL cli_state parameter\n");
		return (1);
	}

	if (aspect)
	{
		if (strncmp("wallet", aspect, strlen("wallet") + 1) == 0)
		{
			printf(TAB4 "Discard session wallet? ");
			if (getConsent(cli_state))
				return (cmd_new_wallet(cli_state));
			printf(TAB4 "Aborted wallet replacement\n");
			return (0);
		}
		if (strncmp("mempool", aspect, strlen("mempool") + 1) == 0)
		{
			printf(TAB4 "Discard session mempool? ");
			if (getConsent(cli_state))
				return (cmd_new_mempool(cli_state));
			printf(TAB4 "Aborted mempool replacement\n");
			return (0);
		}
		if (strncmp("blockchain", aspect, strlen("blockchain") + 1) == 0)
		{
			printf(TAB4 "Discard session blockchain? ");
			if (getConsent(cli_state))
				return (cmd_new_blockchain(cli_state));
			printf(TAB4 "Aborted blockchain replacement\n");
			return (0);
		}
	}

	printf(TAB4 "Usage: new wallet/mempool/blockchain\n");
	return (1);
}
