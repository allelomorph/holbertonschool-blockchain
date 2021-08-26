#include "hblk_cli.h"
/* isatty */
#include <unistd.h>
/* printf perror getline fprintf */
#include <stdio.h>
/* free */
#include <stdlib.h>
/* strncmp strlen */
#include <string.h>


/**
 * CLILoop - primary REPL loop in hblk_cli
 *
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 */
void CLILoop(cli_state_t *cli_state)
{
	char *line = NULL;
	st_list_t *st_list = NULL;

	if (!cli_state)
	{
		fprintf(stderr, "CLILoop: NULL paramter\n");
		return;
	}

	cli_state->loop_ct = 1;
	do {
		if (!line)
			setScriptFd(cli_state);

		line = _readline(NULL, cli_state);
		if (!line)
			unsetScriptFd(cli_state);
		else
		{
			st_list = lineToSTList(line, cli_state);
			if (st_list)
			{
				checkBuiltins(st_list, line, cli_state);
				freeSTList(st_list);
			}
			if (cli_state->exit_code > -1)
			{
				/* only increments after no syntax error */
				cli_state->loop_ct++;
			}

			free(line);
		}

	} while (line); /* freed pointers will not automatically == NULL */
}


/**
 * _readline - prints prompt when in interactive mode, and collects one line of
 *   input: from stdin terminal in interactive mode, or remapped stdin in
 *   non-interactive mode
 *
 * @secondary_prompt: used when collecting secondary input; if NULL, defaults
 *   to main CLI prompt for command input
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 *
 * Return: buffer containing line of input from stdin, NULL on failure or when
 *   EOF reached
 */
char *_readline(char *secondary_prompt, cli_state_t *cli_state)
{
	char *line_buf = NULL;
	ssize_t read_bytes = 0;
	size_t buf_bytes = 0; /* must be intialized or segfault on getline */
	int tty;

	tty = isatty(STDIN_FILENO);
	if (tty)
	{
		if (secondary_prompt)
			printf("%s ", secondary_prompt);
		else
			printf("%s ", CLI_PRIMARY_PROMPT);
		fflush(stdout);
	}
	else if (errno != ENOTTY)
	{
		perror("_readline: isatty error");
		cli_state->exit_code = -1;
		return (NULL);
	}

	if ((read_bytes = getline(&line_buf, &buf_bytes, stdin)) == -1)
	{ /* getline failure or EOF reached / ctrl+d entered by user */
		if (line_buf)
			free(line_buf);
		if (errno == EINVAL)
		{
			perror("_readline: getline error");
			cli_state->exit_code = -1;
			return (NULL);
		}
		if (tty) /* no errors, ctrl+d state */
			printf("\n"); /* final \n to exit prompt */
		return (NULL); /* signal end of loop */
	}

	/* replace newline char at end */
	line_buf[read_bytes - 1] = '\0';
	return (line_buf);
}


/**
 * checkBuiltins - compares first syntax token of a command against the names
 *   of CLI builtins and executes one if matched
 *
 * @st_head: head of syntax token list of current command
 * @line: user input line, included here to be freed by cmd_exit
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 */
void checkBuiltins(st_list_t *st_head, char *line, cli_state_t *cli_state)
{
	char *arg_0 = NULL, *arg_1 = NULL, *arg_2 = NULL;

	if (!st_head || !line || !cli_state)
	{
		fprintf(stderr, "checkBuiltins: NULL paramter(s)\n");
		cli_state->exit_code = -1;
		return;
	}

	arg_0 = st_head->token;
	if (st_head->next)
	{
		arg_1 = st_head->next->token;
		if (st_head->next->next)
			arg_2 = st_head->next->next->token;
	}

	/* wallet_load wallet_save send mine info load save wallet_info? help? exit? */
	/* cmd_wallet_load cmd_wallet_save cmd_send cmd_mine cmd_info cmd_load cmd_save */

	if (strncmp("wallet_load", arg_0, strlen("wallet_load") + 1) == 0)
		/* cli_state->exit_code = cmd_wallet_load(arg_1, state); */
		printf("Will call builtin %s with param(s): '%s' '%s'\n", arg_0, arg_1, arg_2);
	else if (strncmp("wallet_save", arg_0, strlen("wallet_save") + 1) == 0)
		/* cli_state->exit_code = cmd_wallet_save(arg_1, state); */
		printf("Will call builtin %s with param(s): '%s' '%s'\n", arg_0, arg_1, arg_2);
	else if (strncmp("send", arg_0, strlen("send") + 1) == 0)
		/* cli_state->exit_code = cmd_send(arg_1, arg_2, cli_state); */
		printf("Will call builtin %s with param(s): '%s' '%s'\n", arg_0, arg_1, arg_2);
	else if (strncmp("mine", arg_0, strlen("mine") + 1) == 0)
		/* cli_state->exit_code = cmd_mine(cli_state);*/
		printf("Will call builtin %s with param(s): '%s' '%s'\n", arg_0, arg_1, arg_2);
	else if (strncmp("info", arg_0, strlen("info") + 1) == 0)
		/* cli_state->exit_code = cmd_info(cli_state); */
		printf("Will call builtin %s with param(s): '%s' '%s'\n", arg_0, arg_1, arg_2);
	else if (strncmp("load", arg_0, strlen("load") + 1) == 0)
		/* cli_state->exit_code = cmd_load(arg_1, cli_state); */
		printf("Will call builtin %s with param(s): '%s' '%s'\n", arg_0, arg_1, arg_2);
	else if (strncmp("save", arg_0, strlen("save") + 1) == 0)
		/* cli_state->exit_code = cmd_save(arg_1, cli_state); */
		printf("Will call builtin %s with param(s): '%s' '%s'\n", arg_0, arg_1, arg_2);
	else if (strncmp("help", arg_0, strlen("help") + 1) == 0)
		cli_state->exit_code = cmd_help(st_head->next, cli_state);
	/* printf("Will call builtin %s with param(s): '%s' '%s'\n", arg_0, arg_1, arg_2);*/
	else if (strncmp("exit", arg_0, strlen("exit") + 1) == 0)
	        cmd_exit(st_head, line, cli_state);
		/*printf("Will call builtin %s with param(s): '%s' '%s'\n", arg_0, arg_1, arg_2); */
	else
	{
		fprintf(stderr, "%s: %i: '%s': unknwown command\n", cli_state->exec_name, cli_state->loop_ct, arg_0);
		cli_state->exit_code = -1;
	}
}
