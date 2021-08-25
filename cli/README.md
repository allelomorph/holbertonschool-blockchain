# (459) 0x04. Blockchain - CLI
Specializations > System programming & Algorithm > Blockchain

---

### Project author
Alexandre Gautier

### Assignment dates
08-25-2021 to 08-31-2021

### Description
Creating a REPL command line interface for the features built in 0x00 - 0x03, which can make transactions, mine blocks, and save/load the blockchain.

### Provided file(s)
<!--
[`provided/`](./provided/) (all originally [here](https://github.com/holbertonschool/holbertonschool-blockchain/tree/master/blockchain/v0.1/provided)):
* [`_blockchain_destroy.c`](./provided/_blockchain_destroy.c)
* [`_blockchain_print.c`](./provided/_blockchain_print.c)
* [`_endianness.c`](./provided/_endianness.c)
* [`endianness.h`](./provided/endianness.h)
* [`_genesis.c`](./provided/_genesis.c)

[`test/`](./test/):
* [`blockchain_create-main.c`](./test/blockchain_create-main.c)
* [`blockchain_deserialize-main.c`](./test/blockchain_deserialize-main.c)
* [`blockchain_destroy-main.c`](./test/blockchain_destroy-main.c)
* [`blockchain_serialize-main.c`](./test/blockchain_serialize-main.c)
* [`block_create-main.c`](./test/block_create-main.c)
* [`block_destroy-main.c`](./test/block_destroy-main.c)
* [`block_hash-main.c`](./test/block_hash-main.c)
* [`block_is_valid-main.c`](./test/block_is_valid-main.c)
-->
<!-- also add llist/ ? -->

---

## Mandatory Tasks

### :white_large_square: 0. CLI - Create/Load Wallet
At startup, your CLI must either create a wallet (EC key pair), or load it from a folder.

Implement the command `wallet_load`
* Usage: `wallet_load <path>`, where:
    * `<path>` is the path to the folder from which to load the key pair (string)
* Description: Load wallet (EC key pair)
<!--
File(s): [``](./)\ -->

### :white_large_square: 1. CLI - Save Wallet
Implement the command `wallet_save`
* Usage: `wallet_save <path>`, where:
    * `<path>` is the path to the folder in which to save the key pair (string)
* Description: Save wallet (EC key pair)
<!--
File(s): [``](./)\ -->

### :white_large_square: 2. CLI - send
Implement the command `send`
* Usage: `send <amount> <address>`, where:
    * `<amount>` is the number of coins to send (Integer)
    * `<address>` is the EC public key of the receiver
* Description: Send coins
    * Create a new transaction
    * Verify the transaction validity
    * Add transaction to a local list (transaction pool)
    * Do not update list of unspent
<!--
File(s): [``](./)\-->

### :white_large_square: 3. CLI - mine
Implement the command `mine`
* Usage: `mine`
* Description: Mine a block
    * Create a new Block using the Blockchain API
    * **If** transactions are available in the local transaction pool, include the transactions **1 by 1** in the Block
        * Verify the transaction using the list of unspent outputs.
        * If the transaction is not valid, do not include it in the Block, and delete it
        * Update the list of unspent outputs after a transaction is processed
    * Set the difficulty of the Block using the difficulty adjustment method
    * Inject a coinbase transaction **as the first** transaction in the Block
    * Mine the Block (proof of work)
    * Verify Block validity
    * Add the Block to the Blockchain
<!--
File(s): [``](./)\-->

### :white_large_square: 4. CLI - info
Implement the command `info`
* Usage: `info`
* Description: Display information about the Blockchain, at least the following:
    * Display the number of Blocks in the Blockchain
    * Display the number of unspent transaction output
    * Display the number of pending transactions in the local transaction pool
<!--
File(s): [``](./)\-->

### :white_large_square: 5. CLI - load
Implement the command `load`
* Usage: `load <path>`
* Description: Load a Blockchain from a file
    * Override the local Blockchain
<!--
File(s): [``](./)\-->

### :white_large_square: 6. CLI - save
Implement the command `save`
* Usage: `save <path>`
* Description: Save the local Blockchain into a file
    * If the file exists, override it
<!--
File(s): [``](./)\-->

---

## Student
* **Samuel Pomeroy** - [allelomorph](github.com/allelomorph)
