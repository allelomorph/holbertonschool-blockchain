/* TAB4 */
#include "hblk_cli.h"
/* printf fprintf */
#include <stdio.h>
/* strncmp strlen */
#include <string.h>

#define WALLET_LOAD_HELP_SUMMARY TAB4 "wallet_load [<path>] : loads a new wallet into the cli session\n"
#define WALLET_LOAD_HELP WALLET_LOAD_HELP_SUMMARY \
	"\n" TAB4 TAB4 "Attempts to load a wallet from the default path \"./hblk_cli_backup/hblk_cli_wallet\" if <path> is not given. If no wallet file is found, a new wallet (EC key pair) is created for the session. This command is run once automatically at startup. Further calls replace the session wallet with the loaded one upon success.\n\n"

#define WALLET_SAVE_HELP_SUMMARY TAB4 "wallet_save [<path>] : saves wallet from cli session to file\n"
#define WALLET_SAVE_HELP WALLET_SAVE_HELP_SUMMARY \
	"\n" TAB4 TAB4 "Saves current session wallet (EC key pair) to the default path \"./hblk_cli_backup/hblk_cli_wallet\" if <path> is not given. On exit from the cli the user is given the option to save the wallet.\n\n"

#define SEND_HELP_SUMMARY TAB4 "send <amount> <address> : sends <amount> coin to <address>\n"
#define SEND_HELP SEND_HELP_SUMMARY \
        "\n" TAB4 TAB4 "<amount> is the number of coins to send (whole integers only)\n" \
        TAB4 TAB4 "<address> is the EC public key of the recipient\n" \
	"\n" TAB4 TAB4 "Creates a new transaction, verifies it (ensures that there are sufficient unspent transactions outputs in the current session wallet to fund the expenditure, and adds it to the mempool, or list of verified transactions awaiting confirmation via mining.\n"

#define MINE_HELP_SUMMARY TAB4 "mine : hashes current mempool into a new block\n"
#define MINE_HELP MINE_HELP_SUMMARY \
        "\n" TAB4 TAB4 "Creates a new block, reverifies all mempool transactions and adds valid ones to the block, adds a coinbase transaction, sets the block difficulty, hashes the block, and adds it to the blockchain.\n"

#define INFO_HELP_SUMMARY TAB4 "info : displays information about the current cli session blockchain.\n"
#define INFO_HELP INFO_HELP_SUMMARY \
        "\n" TAB4 TAB4 "Displays:\n" \
	"\n" TAB4 TAB4 TAB4 " - number of blocks in the blockchain\n" \
        "\n" TAB4 TAB4 TAB4 " - number of unspent transaction outputs\n" \
        "\n" TAB4 TAB4 TAB4 " - number of pending transactions in the local transaction pool\n"

#define LOAD_HELP_SUMMARY TAB4 "load [<path>] : loads a new blockchain into the cli session\n"
#define LOAD_HELP LOAD_HELP_SUMMARY \
	"\n" TAB4 TAB4 "Attempts to load a blockchain from the default path \"./hblk_cli_backup/hblk_cli_blockchain if <path> is not given. Not run automatically at startup, instead each session begins with an empty blockchain.\n\n"

#define SAVE_HELP_SUMMARY TAB4 "save [<path>] : saves the current cli session blockchain to file\n"
#define SAVE_HELP SAVE_HELP_SUMMARY \
	"\n" TAB4 TAB4 "Saves current session blockchain to the default path \"./hblk_cli_backup/hblk_cli_blockchain\" if <path> is not given. On exit from the cli the user is given the option to save the blockchain.\n\n"

#define EXIT_HELP_SUMMARY TAB4 "exit : exits cli session\n"
#define EXIT_HELP EXIT_HELP_SUMMARY \
	"\n" TAB4 TAB4 "Exits cli on both `exit` command and ctrl + d, providing the option save the current wallet and blockchain. Cli will exit with return value of last lexed and parsed command, or -1 for internal failure or -2 for script read failure.\n"

#define GENERAL_HELP_INTRO TAB4 "This command line interface is a means of making use of the simple blockchain features implemented in libhblk_crypto.a and libhblk_blockchain.a to mimic the UXTO transactions and block mining done in Bitcoin core.  The following commands are implemented, and more information for each can be found with `help <command>`:\n\n"
#define GENERAL_HELP WALLET_LOAD_HELP_SUMMARY \
WALLET_SAVE_HELP_SUMMARY \
SEND_HELP_SUMMARY \
MINE_HELP_SUMMARY \
INFO_HELP_SUMMARY \
LOAD_HELP_SUMMARY \
SAVE_HELP_SUMMARY \
EXIT_HELP_SUMMARY \
"\n"


/**
 * cmd_help - executes command `help` from cli REPL
 *
 * @st_list: list of lexed tokens passed as parameters to `help`
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 *
 * Return: 0 on success, 1 on failure
 */
int cmd_help(st_list_t *st_list, cli_state_t *cli_state)
{
	st_list_t *temp;
	int retval = 0;

	if (!cli_state)
	{
		fprintf(stderr, "cmd_help: NULL cli_state\n");
		return (1);
	}

	if (!st_list)
	{
		printf("%s%s", GENERAL_HELP_INTRO, GENERAL_HELP);
		return (retval);
	}

	for (temp = st_list; temp; temp = temp->next)
	{
		if      (strncmp("wallet_load", temp->token,
				 strlen("wallet_load") + 1) == 0)
			printf(WALLET_LOAD_HELP);
		else if (strncmp("wallet_save", temp->token,
				 strlen("wallet_save") + 1) == 0)
			printf(WALLET_SAVE_HELP);
		else if (strncmp("send", temp->token,
				 strlen("send") + 1) == 0)
			printf(SEND_HELP);
		else if (strncmp("mine", temp->token,
				 strlen("mine") + 1) == 0)
			printf(MINE_HELP);
		else if (strncmp("info", temp->token,
				 strlen("info") + 1) == 0)
			printf(INFO_HELP);
		else if (strncmp("load", temp->token,
				 strlen("load") + 1) == 0)
			printf(LOAD_HELP);
		else if (strncmp("save", temp->token,
				 strlen("save") + 1) == 0)
			printf(SAVE_HELP);
		else if (strncmp("exit", temp->token,
				 strlen("wallet_load") + 1) == 0)
			printf(EXIT_HELP);
		else
		{
			fprintf(stderr, TAB4 "help: '%s' is not a valid command\n",
				temp->token);
			retval = 1;
		}
	}

	return (retval);
}
