# (456) 0x01. Blockchain - Data structures
Specializations > System programming & Algorithm > Blockchain

---

### Project author
Alexandre Gautier

### Assignment dates
07-26-2021 to 08-04-2021

### Description
Introduction to data structures used to store each block's info profile and payload of data.

### Provided file(s)
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

<!-- also add llist/ ? -->

---

## Mandatory Tasks

### :white_check_mark: 0. Create blockchain
Write a function that creates a blockchain structure, and initializes it.

* Prototype: `blockchain_t *blockchain_create(void);`
* The blockchain must contain one block upon creation. This block is called the Genesis Block. Its content is static, and pre-defined as follows:
    * index -> 0
    * difficulty -> 0
    * timestamp -> 1537578000
    * nonce -> 0
    * prev_hash -> 0 (times 32)
    * data -> `"Holberton School"` (16 bytes long)
    * hash -> `c52c26c8b5461639635d8edf2a97d48d0c8e0009c817f2b1d3d7ff2f04515803`

File(s): [`blockchain_create.c`](./blockchain_create.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -I. -o blockchain_create-test blockchain_create.c test/blockchain_create-main.c provided/_blockchain_print.c provided/_blockchain_destroy.c -L../../crypto -lhblk_crypto -lllist -lssl -lcrypto -pthread`

### :white_check_mark: 1. Create block
Write a function that creates a block structure and initializes it.

* Prototype: `block_t *block_create(block_t const *prev, int8_t const *data, uint32_t data_len);`, where:
    * `prev` is a pointer to the previous block in the blockchain
    * `data` points to a memory area to duplicate in the block???s `data`
    * `data_len` stores the number of bytes to duplicate in `data`. If `data_len` is bigger than `BLOCKCHAIN_DATA_MAX`, then only `BLOCKCHAIN_DATA_MAX` bytes must be duplicated.
    * The new block???s index must be the previous block???s index + 1
    * The new block???s `difficulty` and `nonce` must both be initialized to 0
    * The new block???s `timestamp` should be initialized using the time(2) system call. It is normal if your value differs from the following example.
    * The new block???s hash must be zeroed
* Your function must return a pointer to the allocated block

File(s): [`block_create.c`](./block_create.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -I. -o block_create-test blockchain_create.c block_create.c test/block_create-main.c provided/_blockchain_print.c provided/_blockchain_destroy.c -L../../crypto -lhblk_crypto -lllist -lssl -lcrypto -pthread`

### :white_check_mark: 2. Delete block
Write a function that deletes an existing block

* Prototype: `void block_destroy(block_t *block);`, where:
    * `block` points to the block to delete

File(s): [`block_destroy.c`](./block_destroy.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -I. -o block_destroy-test blockchain_create.c block_create.c block_destroy.c test/block_destroy-main.c provided/_blockchain_destroy.c  -L../../crypto -lhblk_crypto -lllist -lssl -lcrypto -pthread`

### :white_check_mark: 3. Delete blockchain
Write a function that deletes an existing blockchain, along with all the blocks it contains

* Prototype: `void blockchain_destroy(blockchain_t *blockchain);`, where:
    * `blockchain` points to the blockchain structure to delete
* All the blocks in the `chain` linked list must be destroyed

File(s): [`blockchain_destroy.c`](./blockchain_destroy.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -I. -o blockchain_destroy-test blockchain_destroy.c blockchain_create.c block_create.c block_destroy.c test/blockchain_destroy-main.c -L../../crypto -lhblk_crypto -lllist -lssl -lcrypto -pthread`

### :white_check_mark: 4. Hash block
Write a function that computes the hash of a block

* Prototype: `uint8_t *block_hash(block_t const *block, uint8_t hash_buf[SHA256_DIGEST_LENGTH]);`, where:
    * `block` points to the block to be hashed
* The resulting hash must be stored in `hash_buf`
* `block->hash` must be left unchanged.
* Your function must return a pointer to `hash_buf`
* You don???t have to allocate any memory dynamically

File(s): [`block_hash.c`](./block_hash.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -I. -o block_hash-test blockchain_create.c block_create.c block_destroy.c blockchain_destroy.c block_hash.c test/block_hash-main.c -L../../crypto -lhblk_crypto -lllist -lssl -lcrypto -pthread`

### :white_check_mark: 5. Save blockchain
Write a function that serializes a blockchain into a file

* Prototype: `int blockchain_serialize(blockchain_t const *blockchain, char const *path);`, where:
    * `blockchain` points to the blockchain to be serialized,
    * `path` contains the path to a file to serialize the blockchain into
* If `path` points to an existing file, it must be overwritten
* Your function must return 0 upon success, or -1 upon failure

#### Format

The resulting file should contain exactly the following:

**File Header**\

Offset | Size (bytes) | Field | Purpose
------ | ------------ | ----- | -------
0x00 | 4 | hblk_magic | Identifies the file as a valid serialized blockchain format. HBLK (48 42 4c 4b) in ASCII; these four bytes constitute the magic number.
0x04 | 3 | hblk_version | Identifies the version at which the blockchain has been serialized. The format is X.Y, where both X and Y are ASCII characters between 0 and 9.
0x07 | 1 | blk_endian | This byte is set to either 1 or 2 to signify little or big endianness, respectively. This affects interpretation of multi-byte fields.
0x08 | 4 | hblk_blocks | Number of blocks in the blockchain. Endianness dependent.
0x0C | ? | blocks | List of blocks

**Block**\

The following table describes how a block is serialized. The blocks are serialized contiguously, the first one starting at offset 0x0C:

Offset | Size (bytes) | Field | Purpose
------ | ------------ | ----- | -------
0x00 | 4 | index | Block index in the blockchain. Endianness dependent.
0x04 | 4 | difficulty | Block difficulty. Endianness dependent.
0x08 | 8 | timestamp | Time at which the block was created (UNIX timestamp). Endianness dependent.
0x10 | 8 | nonce | Salt value used to alter the block hash. Endianness dependent.
0x18 | 32 | prev_hash | Hash of the previous block in the blockchain
0x38 | 4 | data_len | Block data size (in bytes). Endianness dependent.
0x3C | data_len | data | Block data
0x3C + data_len | 32 | hash | Block hash

File(s): [`blockchain_serialize.c`](./blockchain_serialize.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -I. -o blockchain_serialize-test blockchain_create.c block_create.c block_destroy.c blockchain_destroy.c block_hash.c blockchain_serialize.c provided/_endianness.c test/blockchain_serialize-main.c -L../../crypto -lhblk_crypto -lllist -lssl -lcrypto -pthread`

### :white_check_mark: 6. Load blockchain
Write a function that deserializes a blockchain from a file

* Prototype: `blockchain_t *blockchain_deserialize(char const *path);`, where:
    * `path` contains the path to a file to load the blockchain from
* If the file pointed to by `path` doesn???t exist, or you don???t have the permission to open/read it, your function must fail
* Your function must return a pointer to the deserialized blockchain upon success, or `NULL` upon failure
* The format of the file to parse is described in the previous task
* Upon any error, the function must fail and return `NULL`. Error cases:
    * Unable to open/read file
    * Incorrect magic number
    * Version mismatch

File(s): [`blockchain_deserialize.c`](./blockchain_deserialize.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -I. -o blockchain_deserialize-test blockchain_create.c block_create.c block_destroy.c blockchain_destroy.c block_hash.c blockchain_deserialize.c provided/_blockchain_print.c provided/_endianness.c test/blockchain_deserialize-main.c -L../../crypto -lhblk_crypto -lllist -lssl -lcrypto -pthread`

### :white_check_mark: 7. Block validity
Write a function that verifies that a block is valid

* Prototype: `int block_is_valid(block_t const *block, block_t const *prev_block);`, where:
    * `block` points to the block to check
    * `prev_block` points to the previous block in the blockchain, or is `NULL` if `block` is the first block of the chain
* The following requirements must be fulfilled for a block to be valid:
    * `block` should not be `NULL`
    * `prev_block` should be `NULL` ONLY if block???s index is 0
    * If the block???s index is 0, the block should match the Genesis block described in the first task
    * The block???s index must be the previous block???s index, plus 1
    * The computed hash of the previous block must match the one it stores
    * The computed hash of the previous block must match the reference one stored in `block`
    * The computed hash of the block must match the one it stores
    * You don???t have to worry about the `timestamp` and the `difficulty` for this iteration of the blockchain.
    * The block `data` length must not exceed `BLOCKCHAIN_DATA_MAX`

File(s): [`block_is_valid.c`](./block_is_valid.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -I. -o block_is_valid-test blockchain_create.c block_create.c block_destroy.c blockchain_destroy.c block_hash.c block_is_valid.c test/block_is_valid-main.c -L../../crypto -lhblk_crypto -lllist -lssl -lcrypto -pthread`

---

## Student
* **Samuel Pomeroy** - [allelomorph](github.com/allelomorph)
