#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H


/* libllist.so installed in /usr/local/lib/ */
/* E_LLIST */
#include <llist.h>
/* [u]intN_t */
#include <stdint.h>
/* SHA256_DIGEST_LENGTH */
#include "../../crypto/hblk_crypto.h"
/* expected by v0.3 checker mains that only include this header */
#include "transaction/transaction.h"


/* how often (in seconds) a block should be found */
#define BLOCK_GENERATION_INTERVAL 1
/* how often (in blocks) the difficulty should be adjusted */
#define DIFFICULTY_ADJUSTMENT_INTERVAL 5
/* max size (in bytes) of the data payload in a block */
#define BLOCKCHAIN_DATA_MAX 1024

#define HBLK_MAG            "HBLK"
#define HBLK_MAG_LEN        4
#define HBLK_VER            "0.3"
#define HBLK_VER_LEN        3

#define GEN_BLK_TS          1537578000
#define GEN_BLK_DT_BUF      "Holberton School"
#define GEN_BLK_DT_LEN      16
#define GEN_BLK_HSH \
	"\xc5\x2c\x26\xc8\xb5\x46\x16\x39\x63\x5d\x8e\xdf\x2a\x97\xd4\x8d" \
	"\x0c\x8e\x00\x09\xc8\x17\xf2\xb1\xd3\xd7\xff\x2f\x04\x51\x58\x03"
#define GEN_BLK { \
	{ /* info */ \
		0 /* index */, \
		0, /* difficulty */ \
		1537578000, /* timestamp */ \
		0, /* nonce */ \
		{0} /* prev_hash */ \
	}, \
	{ /* data */ \
		"Holberton School", /* buffer */ \
		16 /* len */ \
	}, \
	NULL, /* transactions */ \
	"\xc5\x2c\x26\xc8\xb5\x46\x16\x39\x63\x5d\x8e\xdf\x2a\x97\xd4\x8d" \
	"\x0c\x8e\x00\x09\xc8\x17\xf2\xb1\xd3\xd7\xff\x2f\x04\x51\x58\x03" \
}       /* hash */
/*
 * sizeof(block_t.info) + sizeof(block_t.data.len) +
 * GEN_BLK.data.len + sizeof(block_t.hash)
 */
#define GEN_BLK_SERIAL_SZ 108


/**
 * struct blockchain_s - Blockchain structure
 *
 * @chain:   Linked list of Blocks
 * @unspent: Linked list of unspent transaction outputs
 */
typedef struct blockchain_s
{
	llist_t     *chain;
	llist_t     *unspent;
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
 * @info:         Block info
 * @data:         Block data
 * @transactions: List of transactions
 * @hash:         256-bit digest of the Block, to ensure authenticity
 */
typedef struct block_s
{
	block_info_t info; /* This must stay first */
	block_data_t data; /* This must stay second */
	llist_t     *transactions;
	uint8_t      hash[SHA256_DIGEST_LENGTH];
} block_t;

/**
 * struct bc_file_hdr_s - blockchain file header structure
 *
 * @hblk_magic: Identifies the file as a valid serialized blockchain format;
 *   "HBLK" (ASCII 48 42 4c 4b); these four bytes constitute the magic number
 * @hblk_version: Identifies the version at which the blockchain has been
 *   serialized. The format is X.Y, where both X and Y are ASCII characters
 *   between 0 and 9
 * @hblk_endian: This byte is set to either 1 or 2 to signify little or big
 *   endianness, respectively. This affects interpretation of multi-byte fields
 * @hblk_blocks: Number of blocks in the blockchain, endianness dependent
 * @hblk_unspent: Number of unspent transaction outputs in the blockchain,
 *   endianness dependent
 */
typedef struct bc_file_hdr_s
{
	int8_t   hblk_magic[4];
	int8_t   hblk_version[3];
	uint8_t  hblk_endian;
	uint32_t hblk_blocks;
	uint32_t hblk_unspent;
} bc_file_hdr_t;

/**
 * struct buf_info_s - buffer info
 *
 * Description: Used to contain all the parameters necessary for writing to a
 *   uint8_t buffer. Necessary for llist_for_each, as it only takes one void
 *   pointer as an outside parameter to its `action` function.
 *
 * @buf: buffer
 * @sz: buffer size in bytes
 * @idx: buffer index
 */
typedef struct buf_info_s
{
	uint8_t *buf;
	size_t   sz;
	uint32_t idx;
} buf_info_t;


/* blockchain_create.c */
char *strE_LLIST(E_LLIST code);
block_t *newGenesisBlk(void);
blockchain_t *blockchain_create(void);

/* block_create.c */
block_t *block_create(block_t const *prev, int8_t const *data,
		      uint32_t data_len);

/* block_destroy.c */
void block_destroy(block_t *block);

/* blockchain_destroy.c */
void blockchain_destroy(blockchain_t *blockchain);

/* block_hash.c */
/*
 * static int readTxId(transaction_t *tx, unsigned int idx,
 *		    buf_info_t *buf_info);
 */
uint8_t *block_hash(block_t const *block,
		    uint8_t hash_buf[SHA256_DIGEST_LENGTH]);

/* blockchain_serialize.c */
int pathToWriteFD(char const *path);
int writeBlkchnFileHdr(int fd, const blockchain_t *blockchain);
int writeBlock(block_t *block, unsigned int idx, int *fd);
int writeTransaction(transaction_t *tx, unsigned int idx, int *fd);
int writeInput(tx_in_t *tx_in, unsigned int idx, int *fd);
int writeOutput(tx_out_t *tx_out, unsigned int idx, int *fd);
int writeUnspent(unspent_tx_out_t *unspent_tx_out, unsigned int idx, int *fd);
int blockchain_serialize(blockchain_t const *blockchain, char const *path);

/* blockchain_deserialize.c */
int pathToReadFD(char const *path);
int readBlkchnFileHdr(int fd, uint8_t local_endianness,
		      bc_file_hdr_t *header);
void bswapBlock(block_t *block, int32_t *nb_transactions);
int readBlocks(int fd, llist_t *chain,
	       uint8_t local_endianness, bc_file_hdr_t *header);
int readTransactions(int fd, llist_t *transactions,
		     uint32_t nb_transactions, uint8_t local_endianness,
		     uint8_t hblk_endian);
int readInputs(int fd, llist_t *inputs, uint32_t nb_inputs);
int readOutputs(int fd, llist_t *outputs, uint32_t nb_outputs,
		uint8_t local_endianness, uint8_t hblk_endian);
int readUnspent(int fd, llist_t *unspent,
		uint8_t local_endianness, bc_file_hdr_t *header);
blockchain_t *blockchain_deserialize(char const *path);

/* block_is_valid.c */
int miniumumBlkTxs(const block_t *block);
int validateBlockHashes(block_t const *block, block_t const *prev_block);
int block_is_valid(block_t const *block, block_t const *prev_block,
		   llist_t *all_unspent);

/* hash_matches_difficulty.c */
int hash_matches_difficulty(uint8_t const hash[SHA256_DIGEST_LENGTH],
			    uint32_t difficulty);

/* block_mine.c */
void block_mine(block_t *block);

/* blockchain_difficulty.c */
uint8_t adjustDifficulty(const blockchain_t *blockchain,
			 const block_t *latest_blk, uint32_t *difficulty);
uint32_t blockchain_difficulty(blockchain_t const *blockchain);


#endif /* BLOCKCHAIN_H */
