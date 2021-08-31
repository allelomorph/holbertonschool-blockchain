# `hblk_cli` - Holberton blockchain command line interpreter
This command line interface is a means of using the simple blockchain features implemented in projects [(443) 0x00. Blockchain - Crypto](../crypto/), [(456) 0x01. Blockchain - Data structures](../blockchain/v0.1/), [(457) 0x02. Blockchain - Block mining](../blockchain/v0.2/), and [(458) 0x03. Blockchain - Transactions](../blockchain/v0.3/), which mimic the UXTO transactions and block mining done in Bitcoin core. Built to satisfy the [(459) 0x04. Blockchain - CLI](./PROJECT.md) project requirements.

Currently there is no networking functionality to connect nodes, so the blockchain is only a simplified model for educational purposes.

## Development
Version 1.0 of the CLI was built from 08-25-2021 to 08-31-2021 as a solo student project by Samuel Pomeroy - the final stage of a blockchain project arc in his second year of CS education focusing on C, system programming & algorithms. This project drew on experience with other projects from that same curriculum, most notably the `dash` clone [Cascara 2.0](https://github.com/allelomorph/shell_v2).

## Requirements / Build Environment
This project was built mostly conforming to the default Holberton school C guidelines, including the following:
* `Ubuntu 14.04.6 LTS, Trusty Tahr`
* `gcc (Ubuntu 4.8.4-2ubuntu1~14.04.4) 4.8.4`
* gcc flags `-Wall -Werror -Wextra -pedantic` (implicitly `-std=gnu90`)

In addition, the following should be installed:
* `libssl-dev/trusty-updates,trusty-security,now 1.0.1f-1ubuntu2.27`
* `libssl1.0.0/trusty-updates,trusty-security,now 1.0.1f-1ubuntu2.27`
* `openssl/trusty-updates,trusty-security,now 1.0.1f-1ubuntu2.27`

## Installation
First, clone this repository:
```bash
git clone https://github.com/allelomorph/holbertonschool-blockchain
```
Then compile as below.

## Compilation
A Makefile is included, so if you have `make` or a compatible program, while in the cloned repository building will be automated with:
```bash
make
```
If you wish to compile manually, it can be done with:
```bash
gcc -Wall -Werror -Wextra -pedantic -I. -I../blockchain/v0.3/ -I../blockchain/v0.3/transaction/ -I../crypto/ -o hblk_cli hblk_cli.* cli_loop.c scripts.c lexing.c cleanup.c cmd_*.c printing/*.c -L../crypto/ -L../blockchain/v0.3/ -u OpenSSLGlobalCleanup -lhblk_blockchain -lhblk_crypto -lssl -lcrypto -lllist -pthread -Wno-overlength-strings
```

## Use

### Interactive mode
To use in interactive REPL mode, simply lauch the executable with:
```bash
./hblk_cli
```
Interactive mode can be exited with either `exit` or ctrl + d.

### Non-Interactive mode
The shell can also be run non-interactively by either piping in commands via the parent shell:
```bash
echo "<commands>" | ./hblk_cli
```
or by passing a script as an argument:
```bash
./hblk_cli <script name>
```

### Flags
In either mode, flags can be passed to the CLI as arguments:
| flag  | name | effect |
| ----- | ---- | ------ |
| `-w` | wallet | The CLI attempts to load a wallet (EC key pair) from the directory path provided after this flag (see `wallet_load` below.) |
| `-m` | mempool | The CLI attempts to load a mempool file from the path provided after this flag (see `mempool_load` below.) |
| `-b` | blockchain | The CLI attempts to load a blockchain file from the path provided after this flag (see `load` below.)  |

## Parseable Syntax
Currently the CLI has very simple lexing of command lines, and only tokenizes by whitespace, expecting one command per line of input. No control operators or special characters are implemented.

## Builtin Commands
Version 1.0 of the CLI has the following builin commands:

| command | arguments | description |
| ------- | --------- | ----------- |
| wallet_load | \[\<path>\] | loads a new wallet into the CLI session |
| wallet_save | \[\<path>\] | saves wallet from CLI session to a directory |
| send | \<amount> \<address> | sends <amount> coins to <address>; valid transaction enters mempool |
| mine | | hashes current mempool into a new block |
| info | \[\<aspect> / full\] \[full\] | displays information about the current CLI session |
| load | \[\<path>\] | loads a new blockchain into the CLI session |
| save | \[\<path>\] | saves the current CLI session blockchain to file |
| mempool_load | \[\<path>\] | loads a new mempool into the CLI session |
| mempool_save | \[\<path>\] | saves the current CLI session mempool to file |
| help | \[\<command>\] | displays command instructions |
| exit | | exits CLI session |

## Included Files

| file | file contents |
| ---- | ------------- |
| cleanup.c | functions used in freeing CLI data structures |
| cli_loop.c | the primary REPL loop and its subroutines |
| cmd_exit.c | the command `exit` and its subroutines |
| cmd_help.c | the command `help` and its subroutines  |
| cmd_info.c | the command `info` and its subroutines |
| cmd_load.c | the command `load` and its subroutines |
| cmd_mempool_load.c | the command `mempool_load` and its subroutines |
| cmd_mempool_save.c |  the command `mempool_save` and its subroutines |
| cmd_mine.c |  the command `mine` and its subroutines |
| cmd_save.c |  the command `save` and its subroutines |
| cmd_send.c |  the command `send` and its subroutines |
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

---

## Author
* **Samuel Pomeroy** - [allelomorph](github.com/allelomorph)
