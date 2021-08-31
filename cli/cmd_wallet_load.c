/* ->blockchain.h->hblk_crypto.h:EC_KEY EC_KEY_free */
#include "hblk_cli.h"
/* printf fprintf sprintf */
#include <stdio.h>
/* lstat `struct stat` */
#include <sys/types.h>
#include <sys/stat.h>
/* lstat */
#include <unistd.h>
/* strlen strerror */
#include <string.h>
/* PATH_MAX */
#include <linux/limits.h>


/**
 * cmd_wallet_load - loads a "wallet" (EC key pair) from a folder to use in the
 *   current CLI session
 *
 * @path: user provided path to the folder from which to load the key pair,
 *   or NULL for the default folder
 * @arg2: dummy arg to conform to cmd_fp_t typedef
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 *
 * Return: 0 on success, 1 on failure
 */
int cmd_wallet_load(char *path, char *arg2, cli_state_t *cli_state)
{
	struct stat st;
	char file_path[PATH_MAX];
	EC_KEY *prev_wallet;
	int end_slash, stat_ret1, stat_ret2;

	(void)arg2;
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

	end_slash = (path[strlen(path) - 1] == '/');
	sprintf(file_path, "%s%s%s", path, end_slash ? "" : "/", PUB_FILENAME);
	stat_ret1 = lstat(file_path, &st);
	if (stat_ret1 == -1)
	{
		printf(TAB4 TAB4 "'%s': %s\n",
		       file_path, strerror(errno));
	}

	sprintf(file_path, "%s%s%s", path, end_slash ? "" : "/", PRI_FILENAME);
	stat_ret2 = lstat(file_path, &st);
	if (stat_ret2 == -1)
	{
		printf(TAB4 TAB4 "'%s': %s\n",
		       file_path, strerror(errno));
	}

	if (stat_ret1 != -1 && stat_ret2 != -1)
	{
		prev_wallet = cli_state->wallet;
		cli_state->wallet = ec_load(path);
		if (EC_KEY_check_key(cli_state->wallet))
		{
			printf(TAB4 "Loaded wallet from '%s'\n", path);
			EC_KEY_free(prev_wallet);
			return (0);
		}
	}

	printf(TAB4 "Failed to load wallet from '%s'\n", path);
	return (1);
}
