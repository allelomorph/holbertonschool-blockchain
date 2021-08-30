/* BLKCHN_PATH_DFLT */
#include "hblk_cli.h"
/* printf fprintf */
#include <stdio.h>
/* lstat `struct stat` */
#include <sys/types.h>
#include <sys/stat.h>
/* lstat */
#include <unistd.h>
/* strerror */
#include <string.h>


/**
 * cmd_load - loads a blockchain from a given path to use in the current
 *   CLI session
 *
 * @path: user provided path to the file from which to load the blockchain,
 *   or NULL for the default
 * @arg2: dummy arg to conform to cmd_fp_t typedef
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 *
 * Return: 0 on success, 1 on failure
 */
int cmd_load(char *path, char *arg2, cli_state_t *cli_state)
{
	struct stat st;
	blockchain_t *prev_blockchain;

	(void)arg2;
	if (!cli_state)
	{
		fprintf(stderr, "cmd_load: NULL cli_state parameter\n");
		return (1);
	}

	if (!path || !path[0])
	{
		printf(TAB4 "No blockchain file path provided, using default\n");
		path = BLKCHN_PATH_DFLT;
	}

	if (lstat(path, &st) == -1)
	{
		printf(TAB4 "Failed to load blockchain from '%s': %s\n",
		       path, strerror(errno));
		return (1);
	}
	else if (!S_ISREG(st.st_mode))
	{
		printf(TAB4 "Failed to load blockchain from '%s': %s\n",
		       path, "Not a regular file");
		return (1);
	}

	prev_blockchain = cli_state->blockchain;
	cli_state->blockchain = blockchain_deserialize(path);
	if (!cli_state->blockchain)
	{
		printf(TAB4 "Failed to load blockchain from '%s'\n", path);
		return (1);
	}

	printf(TAB4 "Loaded blockchain from '%s'\n", path);
	if (prev_blockchain)
		blockchain_destroy(prev_blockchain);
	return (0);
}
