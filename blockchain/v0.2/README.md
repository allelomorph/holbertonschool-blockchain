# (457) 0x02. Blockchain - Block mining
Specializations > System programming & Algorithm > Blockchain

---

### Project author
Alexandre Gautier

### Assignment dates
08-04-2021 to 08-06-2021

### Description
Introduction to block mining, block difficulty, and proof of work.

### Provided file(s)
[`provided/`](./provided/) (all originally [here](https://github.com/holbertonschool/holbertonschool-blockchain/tree/master/blockchain/v0.2/provided)):
* [`_blockchain_destroy.c`](./provided/_blockchain_destroy.c)
* [`_blockchain_print.c`](./provided/_blockchain_print.c)
* [`_endianness.c`](./provided/_endianness.c)
* [`endianness.h`](./provided/endianness.h)
* [`_genesis.c`](./provided/_genesis.c)

[`test/`](./test/):
* [`blockchain_create-main.c`](./test/blockchain_create-main.c)
* [`blockchain_deserialize-main.c`](./test/blockchain_deserialize-main.c)
* [`blockchain_destroy-main.c`](./test/blockchain_destroy-main.c)
* [`blockchain_difficulty-main.c`](./test/blockchain_difficulty-main.c)
* [`blockchain_serialize-main.c`](./test/blockchain_serialize-main.c)
* [`block_create-main.c`](./test/block_create-main.c)
* [`block_destroy-main.c`](./test/block_destroy-main.c)
* [`block_hash-main.c`](./test/block_hash-main.c)
* [`block_is_valid-main.c`](./test/block_is_valid-main.c)
* [`hash_matches_difficulty-main.c`](./test/hash_matches_difficulty-main.c)
* [`block_mine-main.c`](./test/block_mine-main.c)

<!-- also add llist/ ? -->

---

## Mandatory Tasks

### :white_check_mark: 0. Proof of work
Write a function that checks whether a given hash matches a given difficulty

* Prototype: `int hash_matches_difficulty(uint8_t const hash[SHA256_DIGEST_LENGTH], uint32_t difficulty);`, where:
    * `hash` is the hash to check
    * `difficulty` is the minimum difficulty the hash should match
* Your function must return 1 if the difficulty is respected, or 0 otherwise
* Refer to the concept page attached to this project to understand how the difficulty is represented in a hash

File(s): [`hash_matches_difficulty.c`](./hash_matches_difficulty.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -I. -I../../crypto -o hash_matches_difficulty-test hash_matches_difficulty.c test/hash_matches_difficulty-main.c -L../../crypto -lhblk_crypto -lllist -lssl -lcrypto -pthread`

### :white_check_mark: 1. Block is valid v0.2
Modify the function `int block_is_valid(block_t const *block, block_t const *prev_block);` so that it checks that a block’s hash matches its difficulty

File(s): [`block_is_valid.c`](./block_is_valid.c)\
Compiled: (from v0.1) `gcc -Wall -Wextra -Werror -pedantic -I. -o block_is_valid-test blockchain_create.c block_create.c block_destroy.c blockchain_destroy.c block_hash.c block_is_valid.c test/block_is_valid-main.c -L../../crypto -lhblk_crypto -lllist -lssl -lcrypto -pthread`

### :white_check_mark: 2. Block mining
Write a function that mines a block in order to insert it in the blockchain

* Prototype: `void block_mine(block_t *block);`, where:
    * `block` points to the block to be mined
* Your function must find a hash for `block` that matches its difficulty

File(s): [`block_mine.c`](./block_mine.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -I. -I../../crypto -o block_mine-test blockchain_create.c block_create.c block_destroy.c blockchain_destroy.c block_hash.c block_is_valid.c hash_matches_difficulty.c block_mine.c provided/_genesis.c provided/_blockchain_print.c test/block_mine-main.c -L../../crypto -lhblk_crypto -lllist -lssl -lcrypto -pthread`

### :white_check_mark: 3. Adjust difficulty
Write a function that computes the difficulty to assign to a potential next block in the blockchain.

* Prototype: `uint32_t blockchain_difficulty(blockchain_t const *blockchain);`, where:
    * `blockchain` points to the blockchain to analyze
* Your function must return the difficulty to be assigned to a potential next block in the blockchain
* The following macros must be defined in your header file `blockchain.h`
    * `BLOCK_GENERATION_INTERVAL`
        * Value: 1 (Will be changed during correction)
        * Defines how often (in seconds) a block should be found
    * `DIFFICULTY_ADJUSTMENT_INTERVAL`
        * Value: 5 (Will be changed during correction)
        * Defines how often (in blocks) the difficulty should be adjusted
* If the latest block’s index is a multiple of `DIFFICULTY_ADJUSTMENT_INTERVAL`, AND that it is not the Genesis Block, the difficulty must be adjusted. Otherwise, the difficulty of the latest block in `blockchain` is returned
* Difficulty adjustment:
    * Retrieve the last block for which an adjustment was made (the block with index `<blockchain_size> - DIFFICULTY_ADJUSTMENT_INTERVAL`)
    * Compute the expected elapsed time between the two Blocks
    * Compute the actual elapsed time
    * The difficulty must be incremented if the elapsed time is lower than half the expected elapsed time
    * The difficulty must be decremented if the elapsed time is greater than twice the expected elapsed time
    * The difficulty should not change otherwise

File(s): [`blockchain_difficulty.c`](./blockchain_difficulty.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -I. -I../../crypto -o blockchain_difficulty-test blockchain_create.c block_create.c block_destroy.c blockchain_destroy.c block_hash.c blockchain_difficulty.c provided/_genesis.c provided/_blockchain_print.c test/blockchain_difficulty-main.c -L../../crypto -lhblk_crypto -lllist -lssl -lcrypto -pthread`

---

## Student
* **Samuel Pomeroy** - [allelomorph](github.com/allelomorph)
