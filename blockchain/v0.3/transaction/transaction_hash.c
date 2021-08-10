/* -> blockchain.h -> */
/* llist.h stdint.h hblk_crypto.h */
#include "transaction.h"
/* fprintf */
#include <stdio.h>
/* malloc free */
#include <stdlib.h>
/* memcpy */
#include <string.h>


/**
 * createTxDataBuff - calculates size of, and allocates buffer for, storing
 *   all input/output data used to create a transaction ID
 *
 * @transaction: points to the transaction of which to to compute the hash
 * @input_ct: pointer to count of transaction inputs, modified by reference
 * @output_ct: pointer to count of transaction outputs, modified by reference
 * @tx_IO_buf_sz: pointer to final buffer size, modified by reference
 *
 * Return: pointer to buffer allocated to store all data that will be hashed
 *   into the transaction ID, or NULL on failure
 */
uint8_t *createTxDataBuff(const transaction_t *transaction,
			  int *input_ct, int *output_ct,
			  size_t *tx_IO_buf_sz)
{
	uint8_t *tx_IO_buf;

	if (!transaction || !input_ct || !output_ct)
	{
		fprintf(stderr, "createTxDataBuff: NULL parameter(s)\n");
		return (NULL);
	}

	*input_ct = llist_size(transaction->inputs);
	if (*input_ct == -1)
	{
		fprintf(stderr, "createTxDataBuff: llist_size failure\n");
		return (NULL);
	}

	*output_ct = llist_size(transaction->outputs);
	if (*output_ct == -1)
	{
		fprintf(stderr, "createTxDataBuff: llist_size failure\n");
		return (NULL);
	}

	*tx_IO_buf_sz = ((SHA256_DIGEST_LENGTH * 3) * *input_ct) +
		(SHA256_DIGEST_LENGTH * *output_ct);

	tx_IO_buf = malloc(*tx_IO_buf_sz);
	if (!tx_IO_buf)
	{
		fprintf(stderr, "createTxDataBuff: malloc failure\n");
		return (NULL);
	}

	return (tx_IO_buf);
}


/**
 * populateTxDataBuff - fills a buffer allocated by createTxDataBuff with all
 *   the relevant information about a transaction's inputs and outputs needed
 *   to hash into the transaction ID
 *
 * @transaction: points to the transaction of which to to compute the hash
 * @input_ct: count of transaction inputs
 * @output_ct: count of transaction outputs
 * @tx_IO_buf: pointer to transaction I/O info buffer
 *
 * Return: 0 on success, or 1 on failure
 */
int populateTxDataBuff(const transaction_t *transaction,
		       int input_ct, int output_ct,
		       uint8_t *tx_IO_buf)
{
	int i;
	uint8_t *temp;
	tx_in_t *tx_in;
	tx_out_t *tx_out;

	if (!transaction || !tx_IO_buf)
	{
		fprintf(stderr, "populateTxDataBuff: NULL parameter(s)\n");
		return (1);
	}

	for (i = 0, temp = tx_IO_buf; i < input_ct; i++)
	{
		tx_in = (tx_in_t *)llist_get_node_at(transaction->inputs,
						     (unsigned int)i);
		if (!tx_in)
		{
			fprintf(stderr, "populateTxDataBuff: %s\n",
				"llist_get_node_at failure");
			return (1);
		}
		/* tx_in->block_hash, tx_in->tx_id, tx_in->tx_out_hash */
		memcpy(temp, tx_in, (SHA256_DIGEST_LENGTH * 3));
		temp += (SHA256_DIGEST_LENGTH * 3);
	}
	for (i = 0; i < output_ct; i++)
	{
		tx_out = (tx_out_t *)llist_get_node_at(transaction->outputs,
						       (unsigned int)i);
		if (!tx_out)
		{
			fprintf(stderr, "populateTxDataBuff: %s\n",
				"llist_get_node_at failure");
			return (1);
		}
		memcpy(temp, tx_out->hash, SHA256_DIGEST_LENGTH);
		temp += SHA256_DIGEST_LENGTH;
	}

	return (0);
}


/**
 * transaction_hash - computes the ID (hash) of a transaction.
 *
 * Note: The transaction ID must be a hash of a buffer containing the following
 *   information:
 *   - Each input’s block_hash (32 bytes), tx_id (32 bytes) and tx_out_hash
 *       (32 bytes), followed by
 *   - Each output’s hash (32 bytes)
 *   For example, the buffer for a transaction with 4 inputs and 2 outputs
 *     would be 448 bytes long (32 * 3 * 4 + 32 * 2)
 *
 * @transaction: points to the transaction of which to to compute the hash
 * @hash_buf: buffer in which to store the computed hash
 *
 * Return: pointer to hash_buf, or NULL on failure
 */
uint8_t *transaction_hash(transaction_t const *transaction,
			  uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	uint8_t *tx_IO_buf;
	int input_ct, output_ct;
	size_t tx_IO_buf_sz = 0;

	if (!transaction || !hash_buf)
	{
		fprintf(stderr, "transaction_hash: NULL parameter(s)\n");
		return (NULL);
	}

	tx_IO_buf = createTxDataBuff(transaction, &input_ct, &output_ct,
					   &tx_IO_buf_sz);
	if (!tx_IO_buf)
		return (NULL);

	if (populateTxDataBuff(transaction, input_ct, output_ct,
			       tx_IO_buf) != 0)
	{
		free(tx_IO_buf);
		return (NULL);
	}

	if (!sha256((const int8_t *)tx_IO_buf, tx_IO_buf_sz,
		    hash_buf))
	{
		fprintf(stderr, "transaction_hash: sha256 failure\n");
		free(tx_IO_buf);
		return (NULL);
	}

	free(tx_IO_buf);
	return (hash_buf);
}


/*
 * strE_LLIST needs blockchain_create.c, which is not used in automatic grading
 *		fprintf(stderr, "txDataBuff: llist_size: %s\n",
 *			strE_LLIST(llist_errno));
 */


/*
 * strE_LLIST needs blockchain_create.c, which is not used in automatic grading
 *		fprintf(stderr, "populateTxDataBuff: llist_get_node_at: %s\n",
 *			strE_LLIST(llist_errno));
 */
