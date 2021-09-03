# `hblk_cli` - Holberton blockchain command line interpreter
This command line interface is a means of using the simple blockchain features implemented in projects [(443) 0x00. Blockchain - Crypto](../crypto/), [(456) 0x01. Blockchain - Data structures](../blockchain/v0.1/), [(457) 0x02. Blockchain - Block mining](../blockchain/v0.2/), and [(458) 0x03. Blockchain - Transactions](../blockchain/v0.3/), which mimic the UTXO transactions and block mining done in Bitcoin core, and was built to satisfy the [(459) 0x04. Blockchain - CLI](./PROJECT.md) project requirements.

Currently there is no networking functionality to connect nodes, so the blockchain is only a simplified stand-alone model for educational purposes.

## Development
Version 1.0 of the CLI was built from 08-25-2021 to 08-31-2021 as a solo student project by Samuel Pomeroy. The final stage of a blockchain project arc ending a 2 year CS education focusing on C, system programming & algorithms, this project drew on experience with several others from that same curriculum, most notably the `dash` clone [Cascara 2.0](https://github.com/allelomorph/shell_v2).

## Requirements / Build Environment
This project was built mostly conforming to the default Holberton school C guidelines, including the following:
* `Ubuntu 14.04.6 LTS, Trusty Tahr`
* `git version 1.9.1` or higher
* `GNU Make 3.81` or higher
* `gcc (Ubuntu 4.8.4-2ubuntu1~14.04.4) 4.8.4`
* gcc flags `-Wall -Werror -Wextra -pedantic` (implicitly `-std=gnu90`)

In addition, the following should be installed:
* `libssl-dev/trusty-updates,trusty-security,now 1.0.1f-1ubuntu2.27`
* `libssl1.0.0/trusty-updates,trusty-security,now 1.0.1f-1ubuntu2.27`
* `openssl/trusty-updates,trusty-security,now 1.0.1f-1ubuntu2.27`
* `/usr/local/lib/libllist.so` and `/usr/local/include/llist.h`, see [llist source](https://github.com/holbertonschool/holbertonschool-blockchain/tree/master/llist)

## Installation

### Source
First, clone this repository and enter the repo directory:
```bash
git clone https://github.com/allelomorph/holbertonschool-blockchain
cd holbertonschool-blockchain/
```

### Prerequisities
* If the OpenSSL packages listed above do not appear with `apt list --installed`, use:
```bash
sudo apt-get install libssl1.0.0
sudo apt-get install libssl-dev
```
* `llist.so` and `llist.h` are installed by the Makefile, but can be installed manually with the script in `llist/`:
```bash
cd llist/
./install.bash
```
* `/usr/include/pthread.h` and `/usr/lib/x86_64-linux-gnu/libpthread.so` should come already installed in Ubuntu 14.04

### Compilation
Navigate to `cli/`:
```bash
cd cli/
```
A Makefile is included, so building from `holbertonschool-blockchain/cli/` can be easily automated with:
```bash
make
```
If you wish to compile manually, it can be done with:
```bash
gcc -Wall -Werror -Wextra -pedantic -I. -I../blockchain/v0.3/ -I../blockchain/v0.3/transaction/ -I../crypto/ -o hblk_cli hblk_cli.* cli_loop.c scripts.c lexing.c cleanup.c cmd_*.c printing/*.c -L../crypto/ -L../blockchain/v0.3/ -u OpenSSLGlobalCleanup -lhblk_blockchain -lhblk_crypto -lssl -lcrypto -lllist -pthread -Wno-overlength-strings
```
(The inclusion of `-u OpenSSLGlobalCleanup` and `-Wno-overlength-strings` is explained in the Makefile.)

## Use

### Interactive mode
To use in interactive REPL mode, simply launch the executable with:
```bash
./hblk_cli
```
Interactive mode can be exited with either `exit` or ctrl + d.

### Non-Interactive mode
The CLI can also be run non-interactively by either piping in commands via the parent shell:
```bash
echo "<commands>" | ./hblk_cli
```
or by passing a script as an argument:
```bash
./hblk_cli <script name>
```
For script format, see [Parseable Syntax](#parseable-syntax).

### Flags
In either mode, flags can be passed to the CLI as arguments to determine behavior at startup:
| flag  | name | effect |
| ----- | ---- | ------ |
| `-w` | wallet | CLI attempts to load a wallet (EC key pair) from the directory path provided after this flag (see [`wallet_load`](#wallet_load-path).) |
| `-m` | mempool | CLI attempts to load a mempool file from the path provided after this flag (see [`mempool_load`](#mempool_load-path).) |
| `-b` | blockchain | CLI attempts to load a blockchain file from the path provided after this flag (see [`load`](#load-path).) |

## Parseable Syntax
Currently the CLI has very simple lexing of command lines, and only tokenizes by whitespace, expecting one command per line of input. No control operators or special characters are implemented.

## Builtin Commands
Version 1.0 of the CLI has the following builtin commands:

| command | arguments | summary |
| ------- | --------- | ------- |
| `wallet_load` | \[\<path>\] | loads a new wallet into the CLI session |
| `wallet_save` | \[\<path>\] | saves wallet from CLI session to a directory |
| `send` | \<amount> \<address> | sends \<amount> coin to \<address>; valid transaction enters mempool |
| `mine` | | hashes current mempool into a new block |
| `info` | \[\<aspect> / full\] \[full\] | displays information about the current CLI session |
| `load` | \[\<path>\] | loads a new blockchain into the CLI session |
| `save` | \[\<path>\] | saves the current CLI session blockchain to file |
| `mempool_load` | \[\<path>\] | loads a new mempool into the CLI session |
| `mempool_save` | \[\<path>\] | saves the current CLI session mempool to file |
| `new` | \<aspect>\ | refreshes session data |
| `help` | \[\<command>\] | displays command instructions |
| `exit` | | exits CLI session |

### `wallet_load` \[\<path>\]
Attempts to load a wallet (EC key pair) from the folder at \<path>, or a default path if none is given. If no wallet files are found, a new wallet is created for the session. This command is run once automatically at startup. Further calls replace the session wallet with the loaded one upon success. See [flags](#flags).

### `wallet_save` \[\<path>\]
Saves current session wallet (EC key pair) to the folder at \<path>, or a default if none is given.

### `send` \<amount> \<address>
  * \<amount> is the number of coins to send (currently whole numbers only)
  * \<address> is the EC public key of the recipient

Creates a new transaction, verifies it (ensures that there are sufficient unspent transaction outputs in the current session wallet to fund the expenditure, and adds it to the mempool, or list of verified transactions awaiting confirmation via mining. To send coin, the wallet must first earn coin by mining, or receive coin from other wallets.

### `mine`
Creates a new block, reverifies all mempool transactions, adds the valid ones to the block, adds a coinbase transaction, sets the block difficulty (see [Known Issues](#known-issues--future-improvements),) hashes the block, and adds it to the blockchain.

### `info` \[\<aspect> / full\] \[full\]
* `info wallet`: public key, number of UTXOs, total coin of UTXOs
* `info wallet full`: `info wallet` plus a full list of wallet's unspent UTXOs
* `info mempool`: number of pending transactions in the local memory pool
* `info mempool full`: `info mempool` plus a full list of transactions in the local memory pool
* `info blockchain`: number of UTXOs and blockchain height
* `info blockchain full`: `info blockchain` plus a full list of all UTXOs and blocks in the chain 
* `info`: `info wallet`, `info mempool`, `info blockchain`
* `info full`: `info wallet full`, `info mempool full`, `info blockchain full`

### `load` \[\<path>\]
Attempts to load a blockchain from the default path if \<path> is not given. By default not run automatically at startup, instead each session begins with an empty blockchain. See [flags](#flags).

### `save` \[\<path>\]
Saves current session blockchain to the default path if \<path> is not given.

### `mempool_load` \[\<path>\]
Attempts to load a mempool from the default path if \<path> is not given. By default not run automatically at startup, instead each session begins with an empty mempool. See [flags](#flags).

### `mempool_save` \[\<path>\]
Saves current session mempool to the default path if \<path> is not given.

### `new` \<aspect>\
When used with `wallet`, `blockchain`, or `mempool`, discards the current session data and creates a new replacement.

### `help` \[\<command>\]
Lists all commands when used without an argument.

### `exit`
Crtl + d has the same effect as this command, exiting the CLI with the return value of the last executed  
command, with -1 for internal CLI failure or -2 for script read failure. When in interactive mode, exit provides the option to save the current wallet, mempool, and blockchain.

## Included Files

| file | contents / use |
| ---- | ------------- |
| cleanup.c | functions used in freeing CLI data structures |
| cli_loop.c | the primary REPL loop and its subroutines |
| cmd_exit.c | the command `exit` and its subroutines |
| cmd_help.c | the command `help` and its subroutines  |
| cmd_info.c | the command `info` and its subroutines |
| cmd_load.c | the command `load` and its subroutines |
| cmd_mempool_load.c | the command `mempool_load` and its subroutines |
| cmd_mempool_save.c |  the command `mempool_save` and its subroutines |
| cmd_mine.c | the command `mine` and its subroutines |
| cmd_new.c | the command `new` and its subroutines |
| cmd_save.c | the command `save` and its subroutines |
| cmd_send.c | the command `send` and its subroutines |
| cmd_wallet_load.c | the command `wallet_load` and its subroutines |
| cmd_wallet_save.c | the command `wallet_save` and its subroutines |
| hblk_cli.c | main entry point and its subroutines |
| hblk_cli.h | primary header for CLI functions, macros, and structures |
| help_text.h | auxillary header for string constants used in help text |
| info_formats.h | auxillary header for string constants used in `info` output |
| lexing.c | functions used in lexing commands |
| printing/_blockchain_print.c | functions used in formatted printing of the blockchain |
| printing/_print_all_unspent.c | functions used in formatted printing of unspent transaction outputs |
| printing/_print_hex_buffer.c | functions used in printing byte arrays |
| printing/_transaction_print_brief.c | functions used in abridged formatted printing of transactions |
| printing/_transaction_print.c | functions used in formatted printing of transactions |
| scripts.c | functions used in handling scripts in non-interactive mode |

## Release History
* 1.0 - First release - 31 Aug 2021

## Known Issues / Future Improvements
* Makefile makes help_text.h and info_formats.h dependencies of the final linking of ./hblk_cli, not compilation of cmd_help.o or cmd_info.o, so updating just these headers does not affect help or info output.
* `libhblk_crypto.a` defines a type `sig_t` which conflicts with attempts to `#include <signal.h>`. This prevents creating a signal handler for SIGINT as would normally be done for a CLI shell. In later revisions, `sig_t` could be renamed `sign_t` or `signt_t`.
* `libhblk_crypto.a` functions don't consistently free all data in every possible failure case, which can cause the CLI to leak memory in the rare cases that its own user input filters don't screen out bad inputs.
* `blockchain.h` and `transaction.h`, used in compiling `libhblk_blockchain.a`, have a slightly convoluted circular reference to each other to accommodate the automated grading of previous stages of the blockchain project. The overall referencing of these two headers could be improved.
* Currently there is no way to adjust the expected rate of block generation or the number of new blocks after which to readjust hashing difficulty. These values are set to 1 block per second (`BLOCK_GENERATION_INTERVAL`) and every 5 blocks (`DIFFICULTY_ADJUSTMENT_INTERVAL`) in `blockchain.h`. With a BGI of 1 second, blocks can't be generated fast enough in CLI interactive mode to increase the hashing difficulty, so it always trends to 0. As a result, the starting difficulty is simply set to 0 by the Genesis Block until another solution is found.

---

## Author
* **Samuel Pomeroy** - [allelomorph](github.com/allelomorph)
