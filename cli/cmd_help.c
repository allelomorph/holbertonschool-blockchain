/* TAB4 CMD_*_ARRAY CMD_HELP_SUMMARY_LIST */
#include "hblk_cli.h"
/* GENERAL_HELP_INTRO */
#include "help_text.h"
/* printf fprintf */
#include <stdio.h>
/* strncmp strlen */
#include <string.h>


/**
 * cmd_help - executes command `help` from CLI REPL
 *
 * @command: command for which to print help, print summary of all commands if
 *   NULL
 * @arg2: dummy arg to conform to cmd_fp_t typedef
 * @cli_state: pointer to struct containing information about the CLI and
 *   blockchain in use
 *
 * Return: 0 on success, 1 on failure
 */
int cmd_help(char *command, char *arg2, cli_state_t *cli_state)
{
	char *cmds[CMD_CT] = CMD_NAME_ARRAY;
	char *help[CMD_CT] = CMD_HELP_ARRAY;
	int i;

	(void)arg2;
	if (!cli_state)
	{
		fprintf(stderr, "cmd_help: NULL cli_state\n");
		return (1);
	}

	if (!command)
	{
		printf("%s%s\n", GENERAL_HELP_INTRO, CMD_HELP_SUMMARY_LIST);
		return (0);
	}

	for (i = 0; i < CMD_CT; i++)
	{
		if (strncmp(cmds[i], command, strlen(cmds[i]) + 1) == 0)
		{
			/* printf(RULER); */
			printf("%s", help[i]);
			return (0);
		}
	}

	printf(TAB4 "help: '%s' is not a valid command, %s\n",
	       command, "try one of the following:\n");
	printf(CMD_HELP_SUMMARY_LIST);
	return (1);
}
