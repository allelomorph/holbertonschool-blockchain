#ifndef HBLK_CLI_H
#define HBLK_CLI_H

#include "blockchain.h"
#include <errno.h>


#define CLI_PRIMARY_PROMPT "hblk_cli $"

#define WHITESPACE " \t\v" /* full set " \t\n\v\f\r" */
#define TAB4 "    "
/* used when editing help_text.h and info_formats.h */
#define RULER "1        ^-10      ^-20      ^-30      ^-40      ^-50      " \
	"^-60      ^-70      ^-80\n"

#define FLAG_CT 3
#define FLAG_ARRAY "wmb"

/* add new <aspect> to refresh wallet/mempool/blockchain from CLI? */
/* exit differs from cmd_fp_t typedef and is handled separately */
#define CMD_FP_CT 11 /* not counting exit */
#define CMD_FP_ARRAY { \
	cmd_wallet_load, \
	cmd_wallet_save, \
	cmd_send, \
	cmd_mine, \
	cmd_info, \
	cmd_load, \
	cmd_save, \
	cmd_mempool_load, \
	cmd_mempool_save, \
	cmd_new, \
	cmd_help \
}
#define CMD_CT 12 /* counting exit */
#define CMD_NAME_ARRAY { \
	"wallet_load", \
	"wallet_save", \
	"send", \
	"mine", \
	"info", \
	"load", \
	"save", \
	"mempool_load", \
	"mempool_save", \
	"new", \
	"help", \
	"exit" \
}
#define CMD_HELP_ARRAY { \
	WALLET_LOAD_HELP, \
	WALLET_SAVE_HELP, \
	SEND_HELP, \
	MINE_HELP, \
	INFO_HELP, \
	LOAD_HELP, \
	SAVE_HELP, \
	MEMPOOL_LOAD_HELP, \
	MEMPOOL_SAVE_HELP, \
	NEW_HELP, \
	HELP_HELP, \
	EXIT_HELP \
}
#define CMD_HELP_SUMMARY_LIST (\
	WALLET_LOAD_HELP_SUMMARY \
	WALLET_SAVE_HELP_SUMMARY \
	SEND_HELP_SUMMARY \
	MINE_HELP_SUMMARY \
	INFO_HELP_SUMMARY \
	LOAD_HELP_SUMMARY \
	SAVE_HELP_SUMMARY \
	MEMPOOL_LOAD_HELP_SUMMARY \
	MEMPOOL_SAVE_HELP_SUMMARY \
	NEW_HELP_SUMMARY \
	HELP_HELP_SUMMARY \
	EXIT_HELP_SUMMARY)

#define HMPL_MAG "HMPL"
#define HMPL_MAG_LEN 4

#define SAVE_DIR_DFLT "hblk_save/"
#define WALLET_DIR_DFLT SAVE_DIR_DFLT "wallet/"
#define WALLET_PUB_PATH_DFLT (WALLET_DIR_DFLT PUB_FILENAME)
#define WALLET_PRI_PATH_DFLT (WALLET_DIR_DFLT PRI_FILENAME)
#define MEMPOOL_PATH_DFLT SAVE_DIR_DFLT "save.hmpl"
#define BLKCHN_PATH_DFLT SAVE_DIR_DFLT "save.hblk"


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
 * @exec_name: path of cli executable
 * @arg_wallet: path of wallet directory passed from command line
 * @arg_mempool: path of mempool file passed from command line
 * @arg_blockchain: path of blockchain file passed from command line
 * @arg_script: path of non-interactive mode script passed from command line
 * @arg_script_fd: -1 by default, or fd of main(argv[1]) script
 * @stdin_bup: -1 by default, stores backup of stdin fd when executing script
 * @loop_ct: amount of lines entered and fully parsed
 * @exit_code: exit code/return value of last builtin/fail condition
 * @wallet: EC keypair for user, created for session or loaded from file
 * @blockchain: blockchain created in session or loaded from file
 * @mempool: list of transactions that have been verified but not
 *   confirmed(mined)
 * @unspent_cache: temproary list of unspent outputs used to track pending
 *   transactions in the mempool, refreshed every time a block is mined and
 *   canonical utxo list blockchain->unspent is updated
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
	llist_t *unspent_cache;
} cli_state_t;

/**
 * struct mpl_file_hdr_s - mempool file header structure
 *
 * @hmpl_magic: Identifies the file as a valid serialized mempool format;
 *   "HMPL" (ASCII 48 4d 50 4c); these four bytes constitute the magic number
 * @hblk_version: Identifies the version at which compatible blockchains have
 *   been serialized. The format is X.Y, where both X and Y are ASCII
 *   characters between 0 and 9.
 * @hmpl_endian: This byte is set to either 1 or 2 to signify little or big
 *   endianness, respectively. This affects interpretation of multi-byte fields
 * @hmpl_txs: Number of transactions in the mempool, endianness dependent
 */
typedef struct mpl_file_hdr_s
{
	int8_t   hmpl_magic[4];
	int8_t   hblk_version[3];
	uint8_t  hmpl_endian;
	uint32_t hmpl_txs;
} mpl_file_hdr_t;

/**
 * struct mtb_info_s - mempool-to-block info
 *
 * Description: Used to contain all the parameters necessary for validating
 *   mempool transactions and adding them to the new block to be mined.
 *   Necessary for llist_for_each, as it only takes one void pointer as an
 *   outside parameter to its `action` function.
 *
 * @new_block: newly created block to contain the mempool transactions
 * @unspent: list of all unspent outputs in the blockchain
 */
typedef struct mtb_info_s
{
	block_t *new_block;
	llist_t *unspent;
} mtb_info_t;

/* function pointer type for all builtin commands other than `exit` */
typedef int (*cmd_fp_t)(char *arg1, char *arg2, cli_state_t *cli_state);


/* hblk_cli.c */
cli_state_t *initCLIState(void);
void parseArgs(int argc, char *argv[], cli_state_t *cli_state);
/*
 * static int copyUnspent(unspent_tx_out_t *utxo,
 *		          unsigned int idx, llist_t *unspent_cache)
 */
int refreshUnspentCache(cli_state_t *cli_state);
void initSession(cli_state_t *cli_state);
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

/* cmd_load.c */
int cmd_load(char *path, char *arg2, cli_state_t *cli_state);

/* cmd_save.c */
int cmd_save(char *path, char *arg2, cli_state_t *cli_state);

/* cmd_mempool_load.c */
int cmd_mempool_load(char *path, char *arg2, cli_state_t *cli_state);

/* cmd_mempool_save.c */
int cmd_mempool_save(char *path, char *arg2, cli_state_t *cli_state);

/* cmd_info.c */
/*
 * static int findAllSenderUnspent(unspent_tx_out_t *unspent_tx_out,
 *				unsigned int idx, su_info_t *su_info);
 */
int print_info_wallet(int full, llist_t **wallet_unspent,
		      int component, cli_state_t *cli_state);
int print_info_mempool(int component, cli_state_t *cli_state);
int print_info_blockchain(int component, cli_state_t *cli_state);
int print_info(cli_state_t *cli_state);
int print_info_wallet_full(int component, cli_state_t *cli_state);
int print_info_mempool_full(int component, cli_state_t *cli_state);
int print_info_blockchain_full(int component, cli_state_t *cli_state);
int print_info_full(cli_state_t *cli_state);
int cmd_info(char *arg1, char *arg2, cli_state_t *cli_state);

/* cmd_send.c */
/*
 * static int findSenderUnspent(unspent_tx_out_t *unspent_tx_out,
 *			        unsigned int idx, su_info_t *su_info);
 */
uint8_t *pubKeyHexToByteArray(char *address);
int cmd_send(char *amount, char *address, cli_state_t *cli_state);

/* cmd_mine.c */
int cmd_mine(char *arg1, char *arg2, cli_state_t *cli_state);

/* cmd_new.c */
int cmd_new_wallet(cli_state_t *cli_state);
int cmd_new_blockchain(cli_state_t *cli_state);
int cmd_new_mempool(cli_state_t *cli_state);
int cmd_new(char *aspect, char *arg2, cli_state_t *cli_state);

/* printing/_blockchain_print.c */
/*
 * static int _block_print(block_t const *block, unsigned int index,
 *			   char const *indent);
 * static int _block_print_brief(block_t const *block, unsigned int index,
 *			   char const *indent);
 */
void _blockchain_print(blockchain_t const *blockchain);
void _blockchain_print_brief(blockchain_t const *blockchain);

/* printing/_print_all_unspent.c */
/*
 * static int _print_unspent(const unspent_tx_out_t *unspent, unsigned int idx,
 *			     const char *indent);
 */
void _print_all_unspent(llist_t *unspent, char *title);

/* printing/_print_hex_buffer.c */
void _print_hex_buffer(uint8_t const *buf, size_t len);

/* printing/_transaction_print_brief.c */
int _transaction_print_brief_loop(transaction_t const *transaction,
				  unsigned int idx, char const *indent);
void _transaction_print_brief(transaction_t const *transaction);

/* printing/_transaction_print.c */
/*
 * static int _tx_in_print(tx_in_t const *in, unsigned int idx,
 *			   char const *indent);
 * static int _tx_out_print(tx_out_t const *out, unsigned int idx,
 *                         char const *indent);
 */
int _transaction_print_loop(transaction_t const *transaction,
			    unsigned int idx, char const *indent);
void _transaction_print(transaction_t const *transaction);


#endif /* HBLK_CLI_H */
