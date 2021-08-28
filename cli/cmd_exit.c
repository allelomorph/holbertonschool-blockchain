#include "hblk_cli.h"
/* printf fprintf */
#include <stdio.h>
/* free exit */
#include <stdlib.h>
/* isatty */
#include <unistd.h>
/* strtok */
#include <string.h>


/**
 * offerBackupOnExit - offers opportunity to save current session before CLI
 *   exits
 *
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 */
void offerBackupOnExit(cli_state_t *cli_state)
{
	char *consent, *file_path, *consent_tok, *file_path_tok;

	if (!cli_state)
	{
		fprintf(stderr, "offerBackupOnExit: NULL parameter\n");
		cli_state->exit_code = -1;
		return;
	}

	printf(TAB4 "Save your current wallet before exiting? ");
	consent = _readline("(y/n):", cli_state);
	consent_tok = strtok(consent, WHITESPACE);
	if (consent_tok && (consent_tok[0] == 'y' || consent_tok[0] == 'Y') &&
	    consent_tok[1] == '\0')
	{
		file_path = _readline(TAB4 TAB4 \
				      "File path to save wallet?",
				      cli_state);
		file_path_tok = strtok(file_path, WHITESPACE);
		printf(TAB4 TAB4 TAB4 "*test* read path: '%s'\n", file_path_tok);
/*
		NULL file_path handled inside cmd_wallet_save
		cmd_wallet_save(file_path_tok, cli_state);
*/
		free(file_path);
	}
	else
		printf(TAB4 TAB4 "Not saving current wallet.\n");
	free(consent);

	printf(TAB4 "Save the current blockchain before exiting? ");
	consent = _readline("(y/n):", cli_state);
	consent_tok = strtok(consent, WHITESPACE);
	if (consent_tok && (consent_tok[0] == 'y' || consent_tok[0] == 'Y') &&
	    consent_tok[1] == '\0')
	{
		file_path = _readline(TAB4 TAB4	\
				      "File path to save blockchain?",
				      cli_state);
		file_path_tok = strtok(file_path, WHITESPACE);
		printf(TAB4 TAB4 TAB4 "*test* read path: '%s'\n", file_path_tok);
/*
		NULL file_path handled inside cmd_save
		cmd_save(file_path_tok, cli_state);
*/
		if (file_path)
			free(file_path);
	}
	else
		printf(TAB4 TAB4 "Not saving current blockchain.\n");
	free(consent);
}


/**
 * cmd_exit - executes command `exit` from cli REPL
 *
 * @st_list: head of syntax list lexed from current command input line
 * @line: current command line
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 */
void cmd_exit(st_list_t *st_list, char *line, cli_state_t *cli_state)
{
	int retval;

	if (!cli_state)
	{
		fprintf(stderr, "cmd_exit: NULL cli_state parameter\n");
		return;
	}

	if (isatty(STDIN_FILENO))
		offerBackupOnExit(cli_state);

	if (st_list)
		freeSTList(st_list);
	if (line)
		free(line);
	retval = cli_state->exit_code;
	freeCLIState(cli_state);
	exit(retval);
}
