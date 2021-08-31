/* BLKCHN_PATH_DFLT */
#include "hblk_cli.h"
/* printf fprintf */
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
 * cmd_save - saves a blockchain from the current CLI session to a given path
 *
 * @path: user provided path to the file from which to load the blockchain,
 *   or NULL for the default
 * @arg2: dummy arg to conform to cmd_fp_t typedef
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 *
 * Return: 0 on success, 1 on failure
 */
int cmd_save(char *path, char *arg2, cli_state_t *cli_state)
{
	struct stat st;
	char dir_path[PATH_MAX];
	int64_t i;

	(void)arg2;
	if (!cli_state)
	{
		fprintf(stderr, "cmd_save: NULL cli_state parameter\n");
		return (1);
	}

	if (!path || !path[0])
	{
		printf(TAB4 "No blockchain file path provided, using default\n");
		path = BLKCHN_PATH_DFLT;
	}

	/* ENOENT could be the file itself, or a directory in its path */
	/* So we test the parent directory first */
	strcpy(dir_path, path);
	for (i = (int64_t)strlen(dir_path) - 1;
	     i >= 0 && dir_path[i] != '/'; i--)
		dir_path[i] = '\0';

	if (*dir_path && lstat(dir_path, &st) == -1)
	{
		printf(TAB4 "Failed to save blockchain in parent '%s': %s\n",
		       dir_path, strerror(errno));
		return (1);
	}

	if (lstat(path, &st) != -1 && !S_ISREG(st.st_mode))
	{
		printf(TAB4 "Failed to save blockchain to '%s': %s\n",
		       path, "Not a regular file, overwriting is not advised");
		return (1);
	}

	if (!cli_state->blockchain)
	{
		printf(TAB4 "Failed to save blockchain to '%s': %s\n",
		       path, "Session blockchain is missing (NULL)");
		return (1);
	}

	if (blockchain_serialize(cli_state->blockchain, path) != 0)
	{
		printf(TAB4 "Failed to save blockchain to '%s'\n", path);
		return (1);
	}

	printf(TAB4 "Saved blockchain to '%s'\n", path);
	return (0);
}
