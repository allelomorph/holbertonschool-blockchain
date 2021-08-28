#ifndef HBLK_CLI_H
#define HBLK_CLI_H

#include "blockchain.h"
#include <errno.h>


#define WHITESPACE " \t\v" /* full set " \t\n\v\f\r" */
#define CLI_PRIMARY_PROMPT "hblk_cli $"

#define TAB4 "    "

/* new? wallet_info? new_wallet? mempool_load? mempool_save? new_mempool? */
/* exit differs from cmd_ref_t.f_ptr typedef and is handled separately */
#define CMD_FP_CT 3 /* not counting exit */
#define CMD_FP_ARRAY { \
        cmd_wallet_load, \
        cmd_wallet_save, \
		/* cmd_send, */ \
		/* cmd_mine, */ \
		/* cmd_info, */ \
		/* cmd_load, */ \
		/* cmd_save, */ \
        cmd_help \
}
#define CMD_CT 4 /* counting exit */
#define CMD_NAME_ARRAY { \
        "wallet_load", \
	"wallet_save", \
		/* "send", */  \
		/* "mine", */ \
		/* "info", */ \
		/* "load", */ \
		/* "save", */ \
        "help", \
        "exit" \
}
#define CMD_HELP_ARRAY { \
        WALLET_LOAD_HELP, \
        WALLET_SAVE_HELP, \
		/* SEND_HELP, */ \
		/* MINE_HELP, */ \
		/* INFO_HELP, */ \
		/* LOAD_HELP, */ \
		/* SAVE_HELP, */ \
        HELP_HELP, \
        EXIT_HELP \
}
#define CMD_HELP_SUMMARY_LIST WALLET_LOAD_HELP_SUMMARY \
        WALLET_SAVE_HELP_SUMMARY \
		/* SEND_HELP_SUMMARY */ \
		/* MINE_HELP_SUMMARY */ \
		/* INFO_HELP_SUMMARY */ \
		/* LOAD_HELP_SUMMARY */ \
		/* SAVE_HELP_SUMMARY */ \
        HELP_HELP_SUMMARY \
        EXIT_HELP_SUMMARY \


#define SAVE_DIR_DFLT "hblk_save/"
#define WALLET_DIR_DFLT SAVE_DIR_DFLT "wallet/"
#define WALLET_PUB_PATH_DFLT WALLET_DIR_DFLT PUB_FILENAME
#define WALLET_PRI_PATH_DFLT WALLET_DIR_DFLT PRI_FILENAME
#define MEMPOOL_DIR_DFLT SAVE_DIR_DFLT "mempool/"
#define MEMPOOL_PATH_DFLT MEMPOOL_DIR_DFLT "save.hmpl"
#define BLKCHN_DIR_DFLT SAVE_DIR_DFLT "blockchain/"
#define BLKCHN_PATH_DFLT BLKCHN_DIR_DFLT SAVE_DIR_DFLT "save.hblk"


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
 * @wallet: EC keypair for user, created for session or loaded from file
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
	char *arg_wallet;
	char *arg_mempool;
	char *arg_blockchain;
	char *arg_script;
	int arg_script_fd;
	int stdin_bup;
	unsigned int loop_ct;
	int exit_code;
	EC_KEY *wallet;
	blockchain_t *blockchain;
	llist_t *mempool;
} cli_state_t;

/* function pointer type for all builtin commands other than `exit` */
typedef int (*cmd_fp_t)(char *arg1, char *arg2, cli_state_t *cli_state);

/* hblk_cli.c */
cli_state_t *initCLIState(void);
void parseArgs(int argc, char *argv[], cli_state_t *cli_state);
void initWlltBlkchnMpl(cli_state_t *cli_state);
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
int getConsent(cli_state_t *cli_state);
void offerBackupOnExit(cli_state_t *cli_state);
void cmd_exit(st_list_t *st_list, char *line, cli_state_t *cli_state);

/* cmd_help.c */
int cmd_help(char *command, char *arg2, cli_state_t *cli_state);

/* cmd_wallet_load.c */
int cmd_wallet_load(char *path, char *arg2, cli_state_t *cli_state);

/* cmd_wallet_save.c */
int cmd_wallet_save(char *path, char *arg2, cli_state_t *cli_state);

/* _print_hex_buffer.c */
void _print_hex_buffer(uint8_t const *buf, size_t len);

#endif /* HBLK_CLI_H */
