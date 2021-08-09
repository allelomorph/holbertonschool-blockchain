# (458) 0x03. Blockchain - Transactions
Specializations > System programming & Algorithm > Blockchain

---

### Project author
Alexandre Gautier

### Assignment dates
08-09-2021 to 08-25-2021

### Description
Introduction to UXTO and coinbase transactions.

### Provided file(s)
[`provided/`](./provided/) (all originally [here](https://github.com/holbertonschool/holbertonschool-blockchain/tree/master/blockchain/v0.3/provided)):
* [`_blockchain_destroy.c`](./provided/_blockchain_destroy.c)
* [`_blockchain_print.c`](./provided/_blockchain_print.c)
* [`_endianness.c`](./provided/_endianness.c)
* [`endianness.h`](./provided/endianness.h)
* [`_genesis.c`](./provided/_genesis.c)
* [`_print_hex_buffer.c`](./provided/_print_hex_buffer.c)
* [`_transaction_print_brief.c`](./provided/_transaction_print_brief.c)
* [`_transaction_print.c`](./provided/_transaction_print.c)

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

[`transaction/test/`](./transaction/test/):
* [`coinbase_create-main.c`](./transaction/test/coinbase_create-main.c)
* [`coinbase_is_valid-main.c`](./transaction/test/coinbase_is_valid-main.c)
* [`transaction_create-main.c`](./transaction/test/transaction_create-main.c)
* [`transaction_destroy-main.c`](./transaction/test/transaction_destroy-main.c)
* [`transaction_hash-main.c`](./transaction/test/transaction_hash-main.c)
* [`transaction_is_valid-main.c`](./transaction/test/transaction_is_valid-main.c)
* [`tx_in_create-main.c`](./transaction/test/tx_in_create-main.c)
* [`tx_in_sign-main.c`](./transaction/test/tx_in_sign-main.c)
* [`tx_out_create-main.c`](./transaction/test/tx_out_create-main.c)
* [`unspent_tx_out_create-main.c`](./transaction/test/unspent_tx_out_create-main.c)
* [`update_unspent-main.c`](./transaction/test/update_unspent-main.c)

<!-- also add llist/ ? -->

---

## Mandatory Tasks

### :white_large_square: 0. Create transaction output
Write a function that allocates and initializes a transaction output structure

* Prototype: `tx_out_t *tx_out_create(uint32_t amount, uint8_t const pub[EC_PUB_LEN]);`, where:
    * `amount` is the amount of the transaction
    * `pub` is the public key of the transaction recipient
* Your function must return a pointer to the created transaction output upon success, or `NULL` upon failure

File(s): [`transaction/tx_out_create.c`](./transaction/tx_out_create.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -g3 -I. -Itransaction/ -Iprovided/ -I../../crypto -o transaction/tx_out_create-test transaction/tx_out_create.c provided/_print_hex_buffer.c transaction/test/tx_out_create-main.c -L../../crypto -lhblk_crypto -lllist -lssl -lcrypto -pthread`

### :white_large_square: 1. Create unspent transaction output
Write a function that allocates and initializes an unspent transaction output structure

* Prototype: `unspent_tx_out_t *unspent_tx_out_create(uint8_t block_hash[SHA256_DIGEST_LENGTH], uint8_t tx_id[SHA256_DIGEST_LENGTH], tx_out_t const *out);`, where:
    * `block_hash` contains the hash of the block where the referenced transaction output is located
    * `tx_id` contains the hash of a transaction in the block block_hash, where the referenced transaction output is located
    * `out` points to the referenced transaction output
* Your function must return a pointer to the created unspent transaction output upon success, or `NULL` upon failure

File(s): [`transaction/unspent_tx_out_create.c`](./transaction/unspent_tx_out_create.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -g3 -I. -Itransaction/ -Iprovided/ -I../../crypto -o transaction/unspent_tx_out_create-test transaction/tx_out_create.c transaction/unspent_tx_out_create.c provided/_print_hex_buffer.c transaction/test/unspent_tx_out_create-main.c -L../../crypto -lhblk_crypto -lllist -lssl -lcrypto -pthread`

### :white_large_square: 2. Create transaction input
Write a function that allocates and initializes a transaction input structure

* Prototype: `tx_in_t *tx_in_create(unspent_tx_out_t const *unspent);`, where
    * `unspent` points to the unspent transaction output to be converted to a transaction input
* Your function must return a pointer to the created transaction input upon success, or `NULL` upon failure
* The created transaction input’s signature structure must be zeroed

File(s): [`transaction/tx_in_create.c`](./transaction/tx_in_create.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -g3 -I. -Itransaction/ -Iprovided/ -I../../crypto -o transaction/tx_in_create-test transaction/tx_out_create.c transaction/unspent_tx_out_create.c transaction/tx_in_create.c provided/_print_hex_buffer.c transaction/test/tx_in_create-main.c -L../../crypto -lhblk_crypto -lllist -lssl -lcrypto -pthread`

### :white_large_square: 3. Transaction ID
Write a function that computes the ID (hash) of a transaction

* Prototype: `uint8_t *transaction_hash(transaction_t const *transaction, uint8_t hash_buf[SHA256_DIGEST_LENGTH]);`, where:
    * `transaction` points to the transaction to compute the hash of
    * `hash_buf` is a buffer in which to store the computed hash
* `transaction->id` must be left unchanged
* Your function must return a pointer to `hash_buf`
* The transaction ID must be a hash of a buffer containing the following information
    * Each input’s `block_hash` (32 bytes), `tx_id` (32 bytes) and `tx_out_hash` (32 bytes), followed by
    * Each output’s `hash` (32 bytes)
    * For example, the buffer for a transaction with 4 inputs and 2 outputs would be 448 bytes long (32 * 3 * 4 + 32 * 2)

File(s): [`transaction/transaction_hash.c`](./transaction/transaction_hash.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -g3 -I. -Itransaction/ -Iprovided/ -I../../crypto -o transaction/transaction_hash-test transaction/tx_out_create.c transaction/unspent_tx_out_create.c transaction/tx_in_create.c transaction/transaction_hash.c provided/_print_hex_buffer.c transaction/test/transaction_hash-main.c provided/_transaction_print.c -L../../crypto -lhblk_crypto -lllist -lssl -lcrypto -pthread`

### :white_large_square: 4. Sign transaction input
Write a function that signs a transaction input, given the transaction id it is from

* Prototype: `sig_t *tx_in_sign(tx_in_t *in, uint8_t const tx_id[SHA256_DIGEST_LENGTH], EC_KEY const *sender, llist_t *all_unspent);`, where:
    * `in` points to the transaction input structure to sign
    * `tx_id` contains the ID (hash) of the transaction the transaction input to sign is stored in
    * `sender` contains the private key of the recipient of the coins contained in the transaction output referenced by the transaction input
    * `all_unspent` is the list of all unspent transaction outputs to date
* Your function must verify the public key derived from the private key in `sender` matches the public key stored in the transaction output referenced by the transaction input to be signed
* Your function must sign the ID of the transaction `tx_id` the transaction input is stored in, using `sender`'s private key
* Your function must return a pointer to the resulting signature structure upon success, or `NULL` upon failure

File(s): [`transaction/tx_in_sign.c`](./transaction/tx_in_sign.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -g3 -I. -Itransaction/ -Iprovided/ -I../../crypto -o transaction/tx_in_sign-test transaction/tx_out_create.c transaction/unspent_tx_out_create.c transaction/tx_in_create.c transaction/transaction_hash.c transaction/tx_in_sign.c provided/_print_hex_buffer.c transaction/test/tx_in_sign-main.c provided/_transaction_print.c -L../../crypto -lhblk_crypto -lllist -lssl -lcrypto -pthread`

### :white_large_square: 5. Create transaction
Write a function that creates a transaction

* Prototype: `transaction_t *transaction_create(EC_KEY const *sender, EC_KEY const *receiver, uint32_t amount, llist_t *all_unspent);`, where:
    * `sender` contains the private key of the transaction sender
    * `receiver` contains the public key of the transaction recipient
    * `amount` is the amount to send
    * `all_unspent` is the list of all the unspent outputs to date
* Your function must return a pointer to the created transaction upon success, or `NULL` upon failure
* The following steps must be implemented to create a transaction:
    * Select a set of unspent transaction outputs from `all_unspent`, which public keys match `sender`'s private key
    * Create transaction inputs from these selected unspent transaction outputs
    * The function must fail if `sender` does not possess enough coins (total amount of selected unspent outputs lower than `amount`)
    * Create a transaction output, sending `amount` coins to `receiver`'s public key
    * If the total amount of the selected unspent outputs is greater than `amount`, create a second transaction output, sending the leftover back to `sender`
    * Compute transaction ID (hash)
    * Sign transaction inputs using the previously computed transaction ID

File(s): [`transaction/transaction_create.c`](./transaction/transaction_create.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -g3 -I. -Itransaction/ -Iprovided/ -I../../crypto -o transaction/transaction_create-test transaction/tx_out_create.c transaction/unspent_tx_out_create.c transaction/tx_in_create.c transaction/transaction_hash.c transaction/tx_in_sign.c transaction/transaction_create.c provided/_print_hex_buffer.c provided/_transaction_print.c transaction/test/transaction_create-main.c -L../../crypto -lhblk_crypto -lllist -lssl -lcrypto -pthread`

### :white_large_square: 6. Transaction validation
Write a function that checks whether a transaction is valid

* Prototype: `int transaction_is_valid(transaction_t const *transaction, llist_t *all_unspent);`, where:
    * `transaction` points to the transaction to verify
    * `all_unspent` is the list of all unspent transaction outputs to date
* Your function must return 1 if the transaction is valid, and 0 otherwise
* The transaction must verify the following:
    * The computed hash of the transaction must match the hash stored in it
    * Each input in the transaction must refer to an unspent output in `all_unspent`
    * Each input’s signature must be verified using the public key stored in the referenced unspent output
    * The total amount of inputs must match the total amount of outputs

File(s): [`transaction/transaction_is_valid.c`](./transaction/transaction_is_valid.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -g3 -I. -Itransaction/ -Iprovided/ -I../../crypto -o transaction/transaction_is_valid-test transaction/tx_out_create.c transaction/unspent_tx_out_create.c transaction/tx_in_create.c transaction/transaction_hash.c transaction/tx_in_sign.c transaction/transaction_is_valid.c provided/_print_hex_buffer.c transaction/test/transaction_is_valid-main.c provided/_transaction_print.c -L../../crypto -lhblk_crypto -lllist -lssl -lcrypto -pthread`

### :white_large_square: 7. Coinbase transaction
Write a function that creates a coinbase transaction

* Prototype: `transaction_t *coinbase_create(EC_KEY const *receiver, uint32_t block_index);`, where:
    * `receiver` contains the public key of the miner, who will receive the coinbase coins
    * `block_index` is the index of the block the coinbase transaction will belong to
* The resulting transaction must contain a single transaction input and a single transaction output
* The transaction input should be zeroed, and not signed. To ensure this transaction input is unique, the block index will be copied in the first 4 bytes of the input’s `tx_out_hash`
* The amount in the transaction output should be exactly `COINBASE_AMOUNT`
* Your function must return a pointer to the created transaction upon success, or `NULL` upon failure

File(s): [`transaction/coinbase_create.c`](./transaction/coinbase_create.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -g3 -I. -Itransaction/ -Iprovided/ -I../../crypto -o transaction/coinbase_create-test transaction/tx_out_create.c transaction/transaction_hash.c transaction/coinbase_create.c provided/_print_hex_buffer.c transaction/test/coinbase_create-main.c provided/_transaction_print.c -L../../crypto -lhblk_crypto -lllist -lssl -lcrypto -pthread`

### :white_large_square: 8. Coinbase transaction validation
Write a function that checks whether a coinbase transaction is valid

* Prototype: `int coinbase_is_valid(transaction_t const *coinbase, uint32_t block_index);`, where:
    * `coinbase` points to the coinbase transaction to verify
    * `block_index` is the index of the block the coinbase transaction will belong to
* Your function must return 1 if the coinbase transaction is valid, and 0 otherwise
* The coinbase transaction must verify the following:
    * The computed hash of the transaction must match the hash stored in it
    * The transaction must contain exactly 1 input
    * The transaction must contain exactly 1 output
    * The transaction input’s `tx_out_hash` first 4 bytes must match the block_index
    * The transaction input’s `block_hash`, `tx_id`, and `signature` must be zeroed
    * The transaction output amount must be exactly `COINBASE_AMOUNT`

File(s): [`transaction/coinbase_is_valid.c`](./transaction/coinbase_is_valid.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -g3 -I. -Itransaction/ -Iprovided/ -I../../crypto -o transaction/coinbase_is_valid-test transaction/tx_out_create.c transaction/transaction_hash.c transaction/coinbase_create.c transaction/coinbase_is_valid.c provided/_print_hex_buffer.c transaction/test/coinbase_is_valid-main.c provided/_transaction_print.c -L../../crypto -lhblk_crypto -lllist -lssl -lcrypto -pthread`

### :white_large_square: 9. Delete transaction
Write a function that deallocates a transaction structure

* Prototype: `void transaction_destroy(transaction_t *transaction);`, where:
    * `transaction` points to the transaction to delete
* Your function must free the content of the transaction and the transaction itself

File(s): [`transaction/transaction_destroy.c`](./transaction/transaction_destroy.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -g3 -I. -Itransaction/ -Iprovided/ -I../../crypto -o transaction/transaction_destroy-test transaction/tx_out_create.c transaction/tx_in_create.c transaction/transaction_hash.c transaction/tx_in_sign.c transaction/transaction_create.c transaction/coinbase_create.c transaction/transaction_destroy.c transaction/test/transaction_destroy-main.c -L../../crypto -lhblk_crypto -lllist -lssl -lcrypto -pthread`

### :white_large_square: 10. Update blockchain and block creation/deletion

* Update the function `block_t *block_create(block_t const *prev, int8_t const *data, uint32_t data_len);`.
    * Your function must now initialize the block’s transaction list to an empty linked list.
* Update the function `void block_destroy(block_t *block);`.
    * Your function must now destroy the block’s transaction list.

File(s): [`block_create.c`](./block_create.c) [`block_destroy.c`](./block_destroy.c)

### :white_large_square: 11. Create/delete list of unspent
Update the functions `blockchain_create` and `blockchain_destroy` to respectively create and delete the linked list `unspent` of unspent transaction outputs.

File(s): [`blockchain_create.c`](./blockchain_create.c) [`blockchain_destroy.c`](./blockchain_destroy.c)

### :white_large_square: 12. Update: Hash block
Update the function `block_hash` to include the list of transactions in a block’s hash.

* Prototype: `uint8_t *block_hash(block_t const *block, uint8_t hash_buf[SHA256_DIGEST_LENGTH]);`, where
    * `block` points to the block to be hashed
* The resulting hash must be stored in `hash_buf`
* `block->hash` must be left unchanged.
* Your function must return a pointer to `hash_buf`
* You might need to use dynamic allocation for this task

File(s): [`block_hash.c`](./block_hash.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -g3 -I. -Itransaction/ -Iprovided/ -I../../crypto -o block_hash-test blockchain_create.c block_create.c block_destroy.c blockchain_destroy.c block_hash.c transaction/tx_out_create.c transaction/tx_in_create.c transaction/transaction_hash.c transaction/coinbase_create.c transaction/transaction_destroy.c provided/_genesis.c provided/_print_hex_buffer.c provided/_blockchain_print.c provided/_transaction_print.c provided/_transaction_print_brief.c test/block_hash-main.c -L../../crypto -lhblk_crypto -lllist -lssl -lcrypto -pthread`

### :white_large_square: 13. Update: block validation
Update the function `block_is_valid` to check that each block’s list of transaction is valid

* Prototype: `int block_is_valid(block_t const *block, block_t const *prev_block, llist_t *all_unspent);`, where:
    * `block`
    * `prev_block`
    * `all_unspent`
* A block must contain at least one transaction (at least the coinbase transaction)
* The first transaction in a block must be a valid coinbase transaction

File(s): [`block_is_valid.c`](./block_is_valid.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -g3 -I. -Itransaction/ -Iprovided/ -I../../crypto -o block_is_valid-test blockchain_create.c block_create.c block_destroy.c blockchain_destroy.c block_hash.c block_is_valid.c hash_matches_difficulty.c blockchain_difficulty.c block_mine.c transaction/tx_out_create.c transaction/unspent_tx_out_create.c transaction/tx_in_create.c transaction/transaction_hash.c transaction/tx_in_sign.c transaction/transaction_create.c transaction/transaction_is_valid.c transaction/coinbase_create.c transaction/coinbase_is_valid.c transaction/transaction_destroy.c provided/_genesis.c provided/_print_hex_buffer.c test/block_is_valid-main.c -L../../crypto -lhblk_crypto -lllist -lssl -lcrypto -pthread`

### :white_large_square: 14. Update all unspent
Write a function that updates the list of all unspent transaction outputs, given a list of processed transactions

* Prototype: `llist_t *update_unspent(llist_t *transactions, uint8_t block_hash[SHA256_DIGEST_LENGTH], llist_t *all_unspent);`, where:
    * `transactions` is the list of validated transactions
    * `block_hash` Hash of the validated block that contains the transaction list `transactions`
    * `all_unspent` is the current list of unspent transaction outputs
* Your function must create and return a new list of unspent transaction outputs
* All transaction inputs from each transaction in `transactions` should not be included in the returned list of unspent transaction outputs
* All transaction outputs from each transaction in `transactions` should be appended in the returned list of unspent transaction outputs
* The list `all_unspent` must be deleted upon success

File(s): [`transaction/update_unspent.c`](./transaction/update_unspent.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -g3 -I. -Itransaction/ -Iprovided/ -I../../crypto -o transaction/update_unspent-test blockchain_create.c block_create.c block_destroy.c blockchain_destroy.c block_hash.c block_is_valid.c hash_matches_difficulty.c blockchain_difficulty.c block_mine.c transaction/tx_out_create.c transaction/unspent_tx_out_create.c transaction/tx_in_create.c transaction/transaction_hash.c transaction/tx_in_sign.c transaction/transaction_create.c transaction/transaction_is_valid.c transaction/coinbase_create.c transaction/coinbase_is_valid.c transaction/transaction_destroy.c transaction/update_unspent.c provided/_genesis.c provided/_print_hex_buffer.c provided/_blockchain_print.c provided/_transaction_print.c provided/_transaction_print_brief.c transaction/test/update_unspent-main.c -L../../crypto -lhblk_crypto -lllist -lssl -lcrypto -pthread`

### :white_large_square: 15. Update: blockchain serialization & deserialization
Update the functions `blockchain_serialize` and `blockchain_deserialize`, to serialize each block’s transactions list.

**blockchain_serialize**

* Prototype: `int blockchain_serialize(blockchain_t const *blockchain, char const *path);`, where:
    * `blockchain` points to the blockchain to be serialized,
    * `path` contains the path to a file to serialize the blockchain into
* If `path` points to an existing file, it must be overwritten
* Your function must return 0 upon success, or -1 upon failure

**blockchain_deserialize**

* Prototype: `blockchain_t *blockchain_deserialize(char const *path);`, where:
    * `path` contains the path to a file to deserialize the blockchain from
* Your function must return the deserialized blockchain upon success, or NULL upon failure

#### Format

The resulting file should contain exactly the following:

**File Header**

Offset | Size (bytes) | Field | Purpose
------ | ------------ | ----- | -------
0x00 | 4 | hblk_magic | Identifies the file as a valid serialized blockchain format. `HBLK` (48 42 4c 4b) in ASCII; these four bytes constitute the magic number.
0x04 | 3 | hblk_version | Identifies the version at which the blockchain has been serialized. The format is X.Y, where both X and Y are ASCII characters between 0 and 9.
0x07 | 1 | hblk_endian | This byte is set to either 1 or 2 to signify little or big endianness, respectively. This affects interpretation of multi-byte fields.
0x08 | 4 | hblk_blocks | Number of blocks in the blockchain. Endianness dependent.
0x0C | 4 | hblk_unspent	| Number of unspent transaction outputs in the blockchain. Endianness dependent.
0x10 | ? | blocks | List of blocks
0x10 + ? | 165 * hblk_unspent | unspent | List of unspent transaction outputs

**Block**

The following table describes how a block is serialized. The blocks are serialized contiguously, the first one starting at offset 0x10:

Offset | Size (bytes) | Field | Purpose
------ | ------------ | ----- | -------
0x00 | 4 | index | Block index in the blockchain. Endianness dependent.
0x04 | 4 | difficulty | Block difficulty. Endianness dependent.
0x08 | 8 | timestamp | Time at which the block was created (UNIX timestamp.) Endianness dependent.
0x10 | 8 | nonce | Salt value used to alter the block hash. Endianness dependent.
0x18 | 32 | prev_hash | Hash of the previous block in the blockchain
0x38 | 4 | data_len | Block data size (in bytes.) Endianness dependent.
0x3C | `data_len` | data | Block data
0x3C + `data_len` | 32 | hash | Block hash
0x5C + `data_len` | 4 | nb_transactions | Number of transactions in the block. Endianness dependent. -1 is for `NULL` (e.g. Genesis Block), 0 is for empty list.
0x60 + `data_len` | ? | transactions | List of transactions

**Transaction**

The following table describes how a transaction is serialized. The transactions of a block are serialized contiguously, the first one starting at offset 0x60 + data_len:

Offset | Size (bytes) | Field | Purpose
------ | ------------ | ----- | -------
0x00 | 32 | id | Transaction ID (hash)
0x20 | 4 | nb_inputs | Number of transaction inputs in the transaction. Endianness dependent.
0x24 | 4 | nb_outputs | Nunber of transaction outputs in the transaction. Endianness dependent
0x28 | 169 * `nb\_inputs` | inputs | List of transaction inputs
0x28 + 169 * `nb\_inputs` | 101 * `nb\_outputs` | outputs | List of transaction outputs

**Transaction input**

The following table describes how a transaction input is serialized. The transaction inputs of a transaction are serialized contiguously, the first one starting at offset 0x28. Each transaction input is stored on exactly 169 bytes:

Offset | Size (bytes) | Field | Purpose
------ | ------------ | ----- | -------
0x00 | 32 | block_hash | Hash of the block in which the consumed transaction output is located
0x20 | 32 | tx_id | ID (hash) of the transaction in which the consumed transaction output is located
0x40 | 32 | tx_out_hash | Hash of the consumed transaction output
0x60 | 72 | sig.sig | Transaction input signature buffer
0xA8 | 1 | sig.len | Transaction input signature length

**Transaction output**

The following table describes how a transaction output is serialized. The transaction outputs of a transaction are serialized contiguously, the first one starting after the last transaction input. Each transaction output is stored on exactly 101 bytes:

Offset | Size (bytes) | Field | Purpose
------ | ------------ | ----- | -------
0x00 | 4 | amount | Transaction output amount. Endianness dependent.
0x04 | 65 | pub	| Recipient’s public key
0x45 | 32 | hash | Transaction output hash

**Unspent transaction output**

The following table describes how an unspent transaction output is serialized. The unspent transaction outputs are serialized contiguously, the first one starting right after the last block serialized. Each serialized unspent transaction output is stored on exactly 165 bytes:

Offset | Size (bytes) | Field | Purpose
------ | ------------ | ----- | -------
0x00 | 32 | block_hash | Hash of the block the referenced transaction output belongs to
0x20 | 32 | tx_id | ID of the transaction the referenced transaction output belongs to
0x40 | 4 | out.amount | Amount of the referenced transaction output. Endianness dependent.
0x44 | 65 | out.pub | Recipient’s public key of the referenced transaction output
0x85 | 32 | out.hash | Hash of the referenced transaction output

File(s): [`blockchain_serialize.c`](./blockchain_serialize.c) [`blockchain_deserialize.c`](./blockchain_deserialize.c)

### :white_large_square: 16. Blockchain library
Write a Makefile that compiles all the previous functions and archives them into a static library for future use.

The library must be called `libhblk_blockchain.a`, and your Makefile must define a rule for this file.

File(s): [`Makefile`](./Makefile)\
Compiled: `make libhblk_blockchain.a`

---

## Student
* **Samuel Pomeroy** - [allelomorph](github.com/allelomorph)
