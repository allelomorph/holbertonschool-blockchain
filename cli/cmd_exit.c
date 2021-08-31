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
 * getConsent - collects consent char from stdin
 *
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 *
 * Return: 1 if first token of input line is "y" or "Y", 0 otherwise or on
 *   failure
 */
int getConsent(cli_state_t *cli_state)
{
	char *consent, *consent_tok;

	if (!cli_state)
	{
		fprintf(stderr, "getConsent: NULL parameter\n");
		return (0);
	}

	consent = _readline("(y/n):", cli_state);
	consent_tok = strtok(consent, WHITESPACE);
	if (consent_tok && (consent_tok[0] == 'y' || consent_tok[0] == 'Y') &&
	    consent_tok[1] == '\0')
	{
		free(consent);
		return (1);
	}

	if (consent)
		free(consent);
	return (0);
}


/**
 * offerBackupOnExit - offers opportunity to save current session before CLI
 *   exits
 *
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 */
void offerBackupOnExit(cli_state_t *cli_state)
{
	char *file_path, *file_path_tok;
	int ret;

	if (!cli_state)
	{
		fprintf(stderr, "offerBackupOnExit: NULL parameter\n");
		cli_state->exit_code = -1;
		return;
	}

	printf(TAB4 "Save your current wallet before exiting? ");
	if (getConsent(cli_state))
	{
		do {
			file_path = _readline(
				TAB4 TAB4 "File path to save wallet?",
				cli_state);
			file_path_tok = strtok(file_path, WHITESPACE);
			ret = cmd_wallet_save(file_path_tok, NULL, cli_state);
			if (file_path)
				free(file_path);
		} while (ret != 0);
	}
	else
		printf(TAB4 TAB4 "Not saving current wallet.\n");

	printf(TAB4 "Save the current mempool before exiting? ");
	if (getConsent(cli_state))
	{
		do {
			file_path = _readline(
				TAB4 TAB4 "File path to save mempool?",
				cli_state);
			file_path_tok = strtok(file_path, WHITESPACE);
			ret = cmd_mempool_save(file_path_tok, NULL, cli_state);
			if (file_path)
				free(file_path);
		} while (ret != 0);
	}
	else
		printf(TAB4 TAB4 "Not saving current mempool.\n");

	printf(TAB4 "Save the current blockchain before exiting? ");
	if (getConsent(cli_state))
	{
		do {
			file_path = _readline(
				TAB4 TAB4 "File path to save blockchain?",
				cli_state);
			file_path_tok = strtok(file_path, WHITESPACE);
			ret = cmd_save(file_path_tok, NULL, cli_state);
			if (file_path)
				free(file_path);
		} while (ret != 0);
	}
	else
		printf(TAB4 TAB4 "Not saving current blockchain.\n");
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

	unsetScriptFd(cli_state);
	if (st_list)
		freeSTList(st_list);
	if (line)
		free(line);
	retval = cli_state->exit_code;
	freeCLIState(cli_state);
	exit(retval);
}
