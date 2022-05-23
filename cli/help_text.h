#ifndef HELP_TEXT_H
#define HELP_TEXT_H

/* TAB4 RULER */
#include "hblk_cli.h"


#define WALLET_LOAD_HELP_SUMMARY TAB4 "wallet_load [<path>] - loads a new " \
	"wallet into the CLI session\n"
#define WALLET_LOAD_HELP "\n" \
	WALLET_LOAD_HELP_SUMMARY \
	"\n" \
	TAB4 TAB4 "Attempts to load a wallet from the folder at <path>," \
	" or a default path\nif none is given. If no wallet files are found," \
	" a new wallet (EC key pair) is\ncreated for the session. This " \
	"command is run once automatically at startup.\nFurther calls " \
	"replace the session wallet with the loaded one upon success.\n" \
	"\n"

#define WALLET_SAVE_HELP_SUMMARY TAB4 "wallet_save [<path>] - saves wallet " \
	"from CLI session to file\n"
#define WALLET_SAVE_HELP "\n" \
	WALLET_SAVE_HELP_SUMMARY \
	"\n" \
	TAB4 TAB4 "Saves current session wallet (EC key pair) to the "	\
	"folder at <path>, or\na default if none is given. On exit from " \
	"the CLI the user is given the option\nto save the wallet.\n" \
	"\n"

#define SEND_HELP_SUMMARY TAB4 "send <amount> <address> - sends <amount> " \
	"coin to <address>\n"
#define SEND_HELP "\n" \
	SEND_HELP_SUMMARY \
	"\n" \
	TAB4 TAB4 "* <amount> is the number of coins to send (whole "	\
	"integers only)\n" \
	TAB4 TAB4 "* <address> is the EC public key of the recipient\n"	\
	"\n" \
	TAB4 TAB4 "Creates a new transaction, verifies it (ensures that" \
	" there are sufficient\nunspent transactions outputs in the current" \
	" session wallet to fund the\nexpenditure, and adds it to the " \
	"mempool, or list of verified transactions\nawaiting confirmation via" \
	" mining.\n" \
	"\n"

#define MINE_HELP_SUMMARY TAB4 "mine - hashes current mempool into a new " \
	"block\n"
#define MINE_HELP "\n" \
	MINE_HELP_SUMMARY \
	"\n" \
	TAB4 TAB4 "Creates a new block, reverifies all mempool "	\
	"transactions, adds the valid\nones to the block, adds a coinbase " \
	"transaction, sets the block difficulty,\nhashes the block, and adds " \
	"it to the blockchain.\n" \
	"\n"

#define INFO_HELP_SUMMARY TAB4 "info [<aspect> / full] [full] - displays " \
	"information about the current CLI\nsession\n"
#define INFO_HELP "\n" INFO_HELP_SUMMARY \
	"\n" \
	TAB4 TAB4 "info wallet: public key, number of UTXOs, total coin of " \
	"UTXOs\n" \
	TAB4 TAB4 "info wallet full: `info wallet` plus a full list of " \
	"wallet's unspent UTXOs\n" \
	TAB4 TAB4 "info mempool: number of pending transactions in the local" \
	" memory pool\n" \
	TAB4 TAB4 "info mempool full: `info mempool` plus a full list of " \
	"transactions in the\nlocal memory pool\n" \
	TAB4 TAB4 "info blockchain: number of UTXOs and blockchain height\n" \
	TAB4 TAB4 "info blockchain full: `info blockchain` plus a full list " \
	"of all UTXOs and\nblocks in the chain\n" \
	TAB4 TAB4 "info: `info wallet`, `info mempool`, `info blockchain`\n" \
	TAB4 TAB4 "info full: `info wallet full`, `info mempool full`,\n" \
	"`info blockchain full`\n" \
	"\n"

#define LOAD_HELP_SUMMARY TAB4 "load [<path>] - loads a new blockchain into " \
	"the CLI session\n"
#define LOAD_HELP "\n" \
	LOAD_HELP_SUMMARY \
	"\n" \
	TAB4 TAB4 "Attempts to load a blockchain from the default path " \
	"if <path> is not\ngiven. Not run automatically at startup, instead " \
	"each session begins with an\nempty blockchain.\n" \
	"\n"

#define SAVE_HELP_SUMMARY TAB4 "save [<path>] - saves the current CLI " \
	"session blockchain to file\n"
#define SAVE_HELP "\n" \
	SAVE_HELP_SUMMARY \
	"\n" \
	TAB4 TAB4 "Saves current session blockchain to the default path" \
	" if <path> is not\ngiven.\n" \
	"\n"

#define MEMPOOL_LOAD_HELP_SUMMARY TAB4 "mempool_load [<path>] - loads a new " \
	"mempool into the CLI session\n"
#define MEMPOOL_LOAD_HELP "\n" \
	MEMPOOL_LOAD_HELP_SUMMARY \
	"\n" \
	TAB4 TAB4 "Attempts to load a mempool from the default path "	\
	"if <path> is not\ngiven. Not run automatically at startup, instead " \
	"each session begins with an\nempty mempool.\n" \
	"\n"

#define MEMPOOL_SAVE_HELP_SUMMARY TAB4 "mempool_save [<path>] - saves the " \
	"current CLI session mempool to file\n"
#define MEMPOOL_SAVE_HELP "\n" \
	MEMPOOL_SAVE_HELP_SUMMARY \
	"\n" \
	TAB4 TAB4 "Saves current session mempool to the default path if " \
	"<path> is not\ngiven.\n" \
	"\n"

#define HELP_HELP_SUMMARY TAB4 "help [<command>] - displays command " \
	"instructions\n"
#define HELP_HELP "\n" \
	HELP_HELP_SUMMARY \
	"\n" \
	TAB4 TAB4 "Lists all commands when used without an argument.\n" \
	"\n"

#define NEW_HELP_SUMMARY TAB4 "new <apsect> - refreshes session data\n"
#define NEW_HELP "\n" \
	NEW_HELP_SUMMARY \
	"\n" \
	TAB4 TAB4 "When used with `wallet`, `blockchain`, or `mempool`, " \
	"discards the current\nsession data and creates a new replacement.\n" \
	"\n"

#define EXIT_HELP_SUMMARY TAB4 "exit - exits CLI session\n"
#define EXIT_HELP "\n" \
	EXIT_HELP_SUMMARY \
	"\n" \
	TAB4 TAB4 "Crtl + d has the same effect as this command, " \
	"exiting the CLI with the\nreturn value of the last executed " \
	"command, or -1 for internal CLI failure or -2\nfor script read " \
	"failure. When in interactive mode, exit provides the option to\nsave" \
	" the current wallet, mempool, and blockchain.\n" \
	"\n"

#define GENERAL_HELP_INTRO "\n" \
	TAB4 "This command line interface is a means " \
	"of using the simple blockchain\nfeatures implemented in " \
	"libhblk_crypto.a and libhblk_blockchain.a, which mimic\nthe UTXO" \
	" transactions and block mining done in Bitcoin core. The " \
	"following\ncommands are implemented, and more information for each" \
	" can be found with\n`help <command>`:\n" \
	"\n"

#endif /* HELP_TEXT_H */
