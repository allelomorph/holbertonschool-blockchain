#include "hblk_cli.h"
/* printf fprintf sprintf */
#include <stdio.h>
/* free */
#include <stdlib.h>
/* lstat `struct stat` */
#include <sys/types.h>
#include <sys/stat.h>
/* lstat isatty */
#include <unistd.h>
/* strtok strerror */
#include <string.h>
/* PATH_MAX */
#include <linux/limits.h>


/**
 * cmd_wallet_load - loads a "wallet" (EC key pair) from a folder to use in the
 *   current CLI session
 *
 * @path: user provided path to the folder from which to load the key pair,
 *   or NULL for the default folder
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 *
 */
int cmd_wallet_load(char *path, cli_state_t *cli_state)
{
/*	struct stat st; */
	uint8_t pub[EC_PUB_LEN];
	char _path[PATH_MAX];
	EC_KEY *prev_wallet;
/*
	char *pub_path = WALLET_PUB_PATH_DFLT, *pri_path = WALLET_PRI_PATH_DFLT;
*/

	if (!cli_state)
	{
		fprintf(stderr, "cmd_wallet_load: NULL cli_state parameter\n");
		return (1);
	}

	if (!path || !path[0])
	{
		printf(TAB4 "No wallet directory path provided, using default\n");
		path = WALLET_DIR_DFLT;
	}
/*
        printf("path:'%s' strlen(path):%lu - 1:%lu path[strlen(path) - 1]:%c (path[strlen(path) - 1] == '/'):%i\n", path, strlen(path), strlen(path) - 1, path[strlen(path) - 1], (path[strlen(path) - 1] == '/'));
*/
	if (path[strlen(path) - 1] != '/')
	{
		sprintf(_path, "%s/", path);
		printf("path:'%s' _path:'%s'\n", path, _path);
		path = _path;
	}

	prev_wallet = cli_state->wallet;
	cli_state->wallet = ec_load(path);
	if (EC_KEY_check_key(cli_state->wallet))
	{
		printf(TAB4 TAB4 "Loaded wallet from '%s'\n", path);
		EC_KEY_free(prev_wallet);
		return (0);
	}

	printf(TAB4 "Failed to load wallet from '%s'\n", path);
	cli_state->wallet = ec_create();
	if (EC_KEY_check_key(cli_state->wallet))
	{
		printf(TAB4 "Created new wallet for session, public key: ");
		_print_hex_buffer(ec_to_pub(cli_state->wallet, pub),
				  EC_PUB_LEN);
		putchar('\n');
		EC_KEY_free(prev_wallet);
		return (0);
	}

	printf(TAB4 "Failed to create new wallet\n");
	return (1);
}
