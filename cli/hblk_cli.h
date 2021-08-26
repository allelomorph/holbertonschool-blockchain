#ifndef HOLBERTON_H
#define HOLBERTON_H

#include "blockchain.h"

#include <errno.h>

#define WHITESPACE " \t\v" /* full set " \t\n\v\f\r" */
#define CLI_PRIMARY_PROMPT "hblk_cli $"
#define WALLET_DIR_DEFAULT "hblk_cli_wallets/"
#define WALLET_FILENAME_DEFAULT "hblk_cli_wallet"
#define WALLET_PATH_DEFAULT WALLET_DIR_DEFAULT WALLET_FILENAME_DEFAULT

#define CMD_CT 8
#define CMD_NAME_ARRAY {"wallet_load", "wallet_save", "send", "mine", "info", \
			"load", "save", /* wallet_info? */"exit"}
#define TAB4 "    "


/**
 * struct st_list_s - singly linked list node to contain a CLI syntax token
 *
 * @token: substring of user input, once stripped of delimiters
 * @next: pointer to next node in list
 *
 * Description: used during lexing to provide a flexible structure for syntax
 *   tokens derived from subdividing a command by delimiters
 */
typedef struct st_list_s
{
	char *token;
	struct st_list_s *next;
} st_list_t;

/**
 * struct cli_state_s - stores state of cli and blockchain in use
 *
 * @exec_name: name of cli executable
 * @arg_script_path: name of main(argv[1]) script if in non-interactive mode
 * @arg_script_fd: -1 by default, or fd of main(argv[1]) script
 * @stdin_bup: -1 by default, stores backup of stdin fd when executing script
 * @loop_ct: amount of lines entered and fully parsed
 * @exit_code: exit code/return value of last builtin/fail condition
 * @user_wallet: EC keypair for user, created for session or loaded from file
 * @blockchain: blockchain created in session or loaded from file
 * @mempool: list of transactions that have been verified but not
 *   confirmed(mined)
 *
 * Description: used to hold anything that needs to be gqlobally visible to
 * various functions to ensure consistent error messages, storage access,
 * and exit codes
 */
typedef struct cli_state_s
{
	char *exec_name;
	char *arg_script_path;
	int arg_script_fd;
	int stdin_bup;
	unsigned int loop_ct;
	int exit_code;
	EC_KEY *user_wallet;
	blockchain_t *blockchain;
	llist_t *mempool;
} cli_state_t;


/* hblk_cli.c */
cli_state_t *initCLIState(char *exec_name);
void freeCLIState(cli_state_t *cli_state);
/* int main(int argc, char **argv); */

/* cli_loop.c */
void CLILoop(cli_state_t *cli_state);
char *_readline(char *secondary_prompt, cli_state_t *cli_state);
void checkBuiltins(st_list_t *st_list, char *line, cli_state_t *cli_state);

/* scripts.c */
void getArgScriptFd(char *file_path, cli_state_t *cli_state);
void setScriptFd(cli_state_t *cli_state);
void unsetScriptFd(cli_state_t *cli_state);

/* lexing.c */
st_list_t *lineToSTList(char *line, cli_state_t *cli_state);

/* cleanup.c */
void freeSTList(st_list_t *st_list);

/* cmd_exit.c */
void cmd_exit(st_list_t *st_list, char *line, cli_state_t *cli_state);

/* cmd_help.c */
int cmd_help(st_list_t *st_list, cli_state_t *cli_state);

/*  */
/*  */
/*  */
/*  */
/*  */
/*  */
/*  */
/*  */

#endif /* HOLBERTON_H */
