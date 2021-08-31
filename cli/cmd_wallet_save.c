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
 * cmd_wallet_save - saves a "wallet" (EC key pair) to a folder
 *
 * @path: user provided path to the folder in which to save the key pair,
 *   or NULL for the default folder
 * @arg2: dummy arg to conform to cmd_fp_t typedef
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 *
 * Return: 0 on success, 1 on failure
 */
int cmd_wallet_save(char *path, char *arg2, cli_state_t *cli_state)
{
	struct stat st = {0};

	(void)arg2;
	if (!cli_state)
	{
		fprintf(stderr, "cmd_wallet_save: NULL cli_state parameter\n");
		return (1);
	}

	if (!path || !path[0])
	{
		printf(TAB4 "No wallet directory path provided, using default\n");
		if ((mkdir(SAVE_DIR_DFLT, S_IRWXU | S_IRWXG |
			   S_IROTH | S_IXOTH) == -1 && errno != EEXIST) ||
		    (mkdir(WALLET_DIR_DFLT, S_IRWXU | S_IRWXG |
			   S_IROTH | S_IXOTH) == -1 && errno != EEXIST))
		{
			printf(TAB4 "Failed to save wallet to '%s': %s\n",
			       WALLET_DIR_DFLT, "Could not create directory");
			return (1);
		}
		path = WALLET_DIR_DFLT;
	}

	if (lstat(path, &st) == -1)
	{
		printf(TAB4 "Failed to save wallet to '%s': %s\n", path,
		       strerror(errno));
		return (1);
	}
	if (!S_ISDIR(st.st_mode))
	{
		printf(TAB4 "Failed to save wallet to '%s': %s\n", path,
		       "Not a directory");
		return (1);
	}
	if (ec_save(cli_state->wallet, path) != 1)
	{
		printf(TAB4 "Failed to save wallet to '%s'\n", path);
		return (1);
	}

	printf(TAB4 "Saved wallet to '%s'\n", path);
	return (0);
}
