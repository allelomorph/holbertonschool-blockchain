#include "hblk_cli.h"
/* open */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
/* dup dup2 close */
#include <unistd.h>
/* fprintf perror */
#include <stdio.h>
/* strdup */
#include <string.h>


/**
 * getArgScriptFd - When a script is passed to cli as a command line arg,
 *   attempts to open the file and store its fd in the cli state
 *
 * @file_path: path of script passed to main as part of argv
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 */
void getArgScriptFd(char *file_path, cli_state_t *cli_state)
{
	if (!file_path || !cli_state)
	{
		fprintf(stderr, "getArgScriptFd: NULL parameter(s)\n");
		cli_state->exit_code = -1;
		return;
	}

	cli_state->arg_script_fd = open(file_path, O_RDONLY);
	if (cli_state->arg_script_fd == -1)
	{
		fprintf(stderr, "%s: %u: Can't open %s\n", cli_state->exec_name,
			cli_state->loop_ct, file_path);

		cli_state->exit_code = -2;
	}
	else
	{
		cli_state->arg_script = strdup(file_path);
		if (!cli_state->arg_script)
		{
			perror("getArgScriptFd: strdup error");
			cli_state->exit_code = -1;
		}
	}
}


/**
 * setScriptFd - backs up inherited stdin fd and maps fd of arg script to stdin
 *
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 */
void setScriptFd(cli_state_t *cli_state)
{
	if (!cli_state)
	{
		fprintf(stderr, "setScriptFd: NULL paramter\n");
		return;
	}

	if (cli_state->arg_script_fd != -1) /* arg script is open */
	{
		/* backup of inherited std fd */
		if ((cli_state->stdin_bup = dup(STDIN_FILENO)) == -1)
			perror("setScriptFds: dup error");
		/* close existing stdin (init script or tty) */
		if (close(STDIN_FILENO) == -1)
			perror("setScriptFds: close error");
		/* map file fd onto std fd */
		if (dup2(cli_state->arg_script_fd, STDIN_FILENO) == -1)
			perror("setScriptFds: dup2 error");
		/* cleanup by closing original once copied */
		if (close(cli_state->arg_script_fd) == -1)
			perror("setScriptFds: close error");
	}
}


/**
 * unsetScriptFd - restores stdin from backup copy after it was remapped by
 *   setScriptFd
 *
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 */
void unsetScriptFd(cli_state_t *cli_state)
{
	if (!cli_state)
	{
		fprintf(stderr, "unsetScriptFd: NULL paramter\n");
		return;
	}

	if (cli_state->arg_script_fd != -1) /* arg script is still open on stdin */
	{
		/* REPL is closing on this loop; restoring fd for safety */
		if (dup2(cli_state->stdin_bup, STDIN_FILENO) == -1)
			perror("unsetScriptFd: dup2 error");
		if (close(cli_state->stdin_bup) == -1)
			perror("unsetScriptFd: close error");

		cli_state->arg_script_fd = -1; /* reset to init value */
		cli_state->stdin_bup = -1; /* reset to init value */
	}
}
