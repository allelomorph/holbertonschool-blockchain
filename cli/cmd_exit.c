#include "hblk_cli.h"
/* perror fprintf */
#include <stdio.h>
/* free */
#include <stdlib.h>
/* strncmp strlen */
#include <string.h>
/* _exit */
#include <unistd.h>


/**
 * cmd_exit - primary REPL loop in hblk_cli
 *
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 */
void cmd_exit(st_list_t *st_list, char *line, cli_state_t *cli_state)
{
	char *consent, *file_path;
	int retval;

	if (!cli_state)
	{
		fprintf(stderr, "cmd_exit: NULL cli_state parameter\n");
		return;
	}

	if (isatty(STDIN_FILENO))
	{
		printf("    Do you wish to save your current wallet before exiting? ");
		consent = _readline("(y/n):", cli_state);
		consent = strtok(consent, WHITESPACE);
		if (consent && (consent[0] == 'y' || consent[0] == 'Y') &&
		    consent[1] == '\0')
		{
			file_path = _readline("        File path at which to save wallet?",
					      cli_state);
			file_path = strtok(file_path, WHITESPACE);
			printf("            *test* read path: '%s'\n", file_path);
/*
			cmd_wallet_save(file_path, cli_state);
*/
			free(file_path);
		}
		else
			printf("        Not saving current wallet.\n");
		free(consent);


		printf("    Do you wish to save the current blockchain before exiting? ");
		consent = _readline("(y/n):", cli_state);
		consent = strtok(consent, WHITESPACE);
		if (consent && (consent[0] == 'y' || consent[0] == 'Y') && consent[1] == '\0')
		{
			file_path = _readline("        File path at which to save blockchain?",
					      cli_state);
			file_path = strtok(file_path, WHITESPACE);
			printf("            *test* read path: '%s'\n", file_path);
/*
			cmd_save(file_path, cli_state);
*/
			free(file_path);
		}
		else
			printf("        Not saving current blockchain.\n");
		free(consent);
	}

	if (st_list)
		freeSTList(st_list);
	if (line)
		free(line);
	retval = cli_state->exit_code;
	freeCLIState(cli_state);
	_exit(retval);
}
