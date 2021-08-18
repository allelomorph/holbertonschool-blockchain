/* block_t */
#include "blockchain.h"
/* fprintf */
#include <stdio.h>
/* sha256 */
#include "../../crypto/hblk_crypto.h"
/* memcpy */
#include <string.h>


/**
 * readTxId - used as `action` for llist_for_each to visit each
 *   transaction in a block, and write the transaction IDs to a data buffer
 *
 * @tx: pointer to transaction in block->transactions list, as iterated
 *   through by llist_for_each
 * @idx: index of `tx` in block->transactions list, as iterated through by
 *   llist_for_each
 * @buf_info: pointer to struct containing all parameters necessary to
 *   incrementally access a uint8_t buffer
 *
 * Return: 0 on incremental success (llist_for_each can continue,)
 *   -2 on failure (-1 reserved for llist_for_each errors)
 */
static int readTxId(transaction_t *tx, unsigned int idx, buf_info_t *buf_info)
{
	uint8_t *ptr;

	(void)idx;

	if (!tx || !buf_info || !buf_info->buf)
	{
		fprintf(stderr, "readTxId: NULL parameter(s)\n");
		return (-2);
	}

	ptr = buf_info->buf + buf_info->idx;
	memcpy(ptr, &(tx->id), SHA256_DIGEST_LENGTH);
	buf_info->idx += SHA256_DIGEST_LENGTH;

	return (0);
}


/**
 * block_hash - computes the hash of a block
 *
 * @block: pointer to the block to be hashed
 * @hash_buf: pointer to buffer for storing hash value
 *
 * Return: pointer to hash_buf, or NULL on failure
 */
uint8_t *block_hash(block_t const *block,
		    uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	buf_info_t buf_info = {NULL, 0, 0};
	int tx_ct;

	if (!block || !hash_buf)
	{
		fprintf(stderr, "block_hash: NULL parameter(s)\n");
		return (NULL);
	}
	/* Genesis block */
	if (block->info.index == 0 && !block->transactions)
	{
		memcpy(hash_buf, GEN_BLK_HSH, SHA256_DIGEST_LENGTH);
		return (hash_buf);
	}
	/* hashed data: block info, len bytes of buffer, tx hashes in series */
	buf_info.sz += sizeof(block_info_t);
	buf_info.sz += block->data.len;
	tx_ct = llist_size(block->transactions);
	if (tx_ct != -1)
		buf_info.sz += (SHA256_DIGEST_LENGTH * tx_ct);
	buf_info.buf = malloc(sizeof(uint8_t) * buf_info.sz);
	if (!buf_info.buf)
	{
		fprintf(stderr, "block_hash: malloc failure\n");
		return (NULL);
	}
	memcpy(buf_info.buf, block, sizeof(block_info_t) + block->data.len);
	buf_info.idx += (sizeof(block_info_t) + block->data.len);
	if (llist_for_each(block->transactions,
			   (node_func_t)readTxId, &buf_info) < 0)
	{
		fprintf(stderr, "block_hash: llist_for_each failure: %s\n",
			strE_LLIST(llist_errno));
		free(buf_info.buf);
		return (NULL);
	}
	if (!sha256((const int8_t *)buf_info.buf, buf_info.sz, hash_buf))
	{
		fprintf(stderr, "block_hash: sha256 failure\n");
		free(buf_info.buf);
		return (NULL);
	}
	free(buf_info.buf);
	return (hash_buf);
}
