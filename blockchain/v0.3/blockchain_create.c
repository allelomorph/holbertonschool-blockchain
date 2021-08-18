/* block_t blockchain_t */
#include "blockchain.h"
/* E_LLIST LLIST_* llist_* */
#include <llist.h>
/* malloc calloc free */
#include <stdlib.h>
/* fprintf */
#include <stdio.h>
/* memcpy */
#include <string.h>


/**
 * strE_LLIST - returns error description corresponding to llist_errno value
 *
 * @code: llist shared library llist_errno value
 *
 * Return: llist error description as string, or generic message on failure
 */
char *strE_LLIST(E_LLIST code)
{
	switch (code)
	{
	case LLIST_SUCCESS:
		return ("success");
	case LLIST_NODE_NOT_FOUND:
		return ("node not found");
	case LLIST_NULL_ARGUMENT:
		return ("NULL parameter(s)");
	case LLIST_OUT_OF_RANGE:
		return ("out of range");
	case LLIST_MALLOC_ERROR:
		return ("malloc error");
	case LLIST_MULTITHREAD_ISSUE:
		return ("multithreading issue");
	default:
		return ("(unknown error code)");
	}
}


/**
 * newGenesisBlk - creates and initializes a Genesis block
 *
 * Note: block content is pre-defined as follows:
 *      index: 0
 *      difficulty: 0
 *      timestamp: 1537578000 (similar to glibc time_t, but unsigned)
 *      nonce: 0
 *	prev_hash: (32 0x00 bytes)
 *	data: "Holberton School" (16 bytes)
 *      hash: c52c26c8b5461639635d8edf2a97d48d0c8e0009c817f2b1d3d7ff2f04515803
 *        (32 bytes)
 *
 * Return: newly allocated Genesis block, or NULL on failure
 */
block_t *newGenesisBlk(void)
{
	block_t *genesis;

	genesis = calloc(1, sizeof(block_t));
	if (!genesis)
	{
		fprintf(stderr, "newGenesisBlk: calloc failure\n");
		return (NULL);
	}

	genesis->info.timestamp = GEN_BLK_TS;

	genesis->data.len = GEN_BLK_DT_LEN;
	memcpy(genesis->data.buffer, GEN_BLK_DT_BUF, GEN_BLK_DT_LEN);

	memcpy(genesis->hash, GEN_BLK_HSH, SHA256_DIGEST_LENGTH);

	return (genesis);
}


/**
 * blockchain_create - creates and initializes a blockchain structure
 *
 * Note: The blockchain contains one block upon creation, known as the
 *   Genesis Block. Its content is pre-defined as follows:
 *      index: 0
 *      difficulty: 0
 *      timestamp: 1537578000 (similar to glibc time_t, but unsigned)
 *      nonce: 0
 *	prev_hash: (32 0x00 bytes)
 *	data: "Holberton School" (16 bytes)
 *      hash: c52c26c8b5461639635d8edf2a97d48d0c8e0009c817f2b1d3d7ff2f04515803
 *        (32 bytes)
 *
 * Return: newly allocated blockchain, or NULL on failure
 */
blockchain_t *blockchain_create(void)
{
	blockchain_t *blockchain;
	block_t *genesis;

	blockchain = malloc(sizeof(blockchain_t));
	if (!blockchain)
	{
		fprintf(stderr, "blockchain_create: malloc failure\n");
		return (NULL);
	}

	blockchain->chain = llist_create(MT_SUPPORT_FALSE);
	if (!(blockchain->chain))
	{
		free(blockchain);
		fprintf(stderr, "blockchain_create: llist_create: %s\n",
			strE_LLIST(llist_errno));
		return (NULL);
	}

	blockchain->unspent = llist_create(MT_SUPPORT_FALSE);
	if (!(blockchain->unspent))
	{
		blockchain_destroy(blockchain);
		fprintf(stderr, "blockchain_create: llist_create: %s\n",
			strE_LLIST(llist_errno));
		return (NULL);
	}

	genesis = newGenesisBlk();
	if (llist_add_node(blockchain->chain, (llist_node_t)genesis,
			   ADD_NODE_FRONT) != 0)
	{
		blockchain_destroy(blockchain);
		fprintf(stderr, "blockchain_create: llist_add_node: %s\n",
			strE_LLIST(llist_errno));
		return (NULL);
	}

	return (blockchain);
}
