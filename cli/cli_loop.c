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

	setScriptFd(cli_state);
	if (cli_state->exit_code != 0)
		return;

	cli_state->loop_ct = 1;
	do {
		line = _readline(NULL, cli_state);
		if (line)
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

	/* if not internal failure and in interactive mode */
	if (cli_state->exit_code > -1 && isatty(STDIN_FILENO))
		offerBackupOnExit(cli_state);

	unsetScriptFd(cli_state);
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
	char *cmd = NULL, *arg_1 = NULL, *arg_2 = NULL;
	char *cmds[CMD_CT] = CMD_NAME_ARRAY;
	cmd_fp_t f_ptrs[CMD_FP_CT] = CMD_FP_ARRAY;
	int i;

	if (!st_head || !line || !cli_state)
	{
		fprintf(stderr, "checkBuiltins: NULL paramter(s)\n");
		cli_state->exit_code = -1;
		return;
	}

	cmd = st_head->token;
	if (st_head->next)
	{
		arg_1 = st_head->next->token;
		if (st_head->next->next)
			arg_2 = st_head->next->next->token;
	}

	if (cmd)
	{
		/* `exit` differs from builtin function pointer type cmd_fp_t */
		if (strncmp("exit", cmd, strlen("exit") + 1) == 0)
			cmd_exit(st_head, line, cli_state);

		for (i = 0; i < CMD_FP_CT; i++)
		{
			if (strncmp(cmds[i], cmd, strlen(cmds[i]) + 1) == 0)
			{
				cli_state->exit_code =
					f_ptrs[i](arg_1, arg_2, cli_state);
				return;
			}
		}

		fprintf(stderr, TAB4 "'%s' is not a valid command, %s\n",
			cmd, "enter `help` for list of commands");
	}
}
