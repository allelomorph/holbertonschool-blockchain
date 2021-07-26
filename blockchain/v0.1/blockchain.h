#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

/* [u]intN_t */
#include <stdint.h>
/* SHA256_DIGEST_LENGTH */
#include <opnessl/sha.h>
/* llist_t defined by linking to liblist.so */


#define BLOCKCHAIN_DATA_MAX 1024


/**
 * struct blockchain_s - Blockchain structure
 *
 * @chain: Linked list of pointers to block_t
 */
typedef struct blockchain_s
{
	llist_t *chain;
} blockchain_t;

/**
 * struct block_info_s - Block info structure
 *
 * @index:      Index of the Block in the Blockchain
 * @difficulty: Difficulty of proof of work (hash leading zero bits)
 * @timestamp:  Time the Block was created at (UNIX timestamp)
 * @nonce:      Salt value used to alter the Block hash
 * @prev_hash:  Hash of the previous Block in the Blockchain
 */
typedef struct block_info_s
{
	/*
	 * The order of the elements in this structure should remain the same.
	 * It was designed so every element of this structure is aligned and
	 * doesn't require padding from the compiler.
	 * Therefore, it is possible to use the structure as an array of char,
	 * on any architecture.
	 */
	uint32_t index;
	uint32_t difficulty;
	uint64_t timestamp;
	uint64_t nonce;
	uint8_t  prev_hash[SHA256_DIGEST_LENGTH];
} block_info_t;

/**
 * struct block_data_s - Block data
 *
 * @buffer: Data buffer
 * @len:    Data size (in bytes)
 */
typedef struct block_data_s
{
	/*
	 * @buffer must stay first, so we can directly use the structure as
	 * an array of char
	 */
	int8_t   buffer[BLOCKCHAIN_DATA_MAX];
	uint32_t len;
} block_data_t;

/**
 * struct block_s - Block structure
 *
 * @info: Block info
 * @data: Block data
 * @hash: 256-bit digest of the Block, to ensure authenticity
 */
typedef struct block_s
{
	block_info_t info; /* This must stay first */
	block_data_t data; /* This must stay second */
	uint8_t      hash[SHA256_DIGEST_LENGTH];
} block_t;


#ifdef DECLARE_PROVIDED
/* provided/_blockchain_destroy.c */
void _blockchain_destroy(blockchain_t *blockchain);

/* provided/_blockchain_print.c */
static void _print_hex_buffer(uint8_t const *buf, size_t len);
static int _block_print(block_t const *block, unsigned int index,
			char const *indent);
void _blockchain_print_brief(blockchain_t const *blockchain);

/* provided/_genesis.c */
block_t const _genesis;
#endif

/* blockchain_create.c */
blockchain_t *blockchain_create(void);

/* block_create.c */
block_t *block_create(block_t const *prev, int8_t const *data,
		      uint32_t data_len);

/* block_destroy.c */
void block_destroy(block_t *block);

/* blockchain_destroy.c */
void blockchain_destroy(blockchain_t *blockchain);

/* block_hash.c */
uint8_t *block_hash(block_t const *block,
		    uint8_t hash_buf[SHA256_DIGEST_LENGTH]);

/* blockchain_serialize.c */
int blockchain_serialize(blockchain_t const *blockchain, char const *path);

/* blockchain_deserialize.c */
blockchain_t *blockchain_deserialize(char const *path);

/* block_is_valid.c */
int block_is_valid(block_t const *block, block_t const *prev_block);


#endif /* BLOCKCHAIN_H */