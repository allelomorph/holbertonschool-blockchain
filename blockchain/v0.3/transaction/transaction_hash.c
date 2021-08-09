#include "transaction.h"
/* fprintf */
#include <stdio.h>


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
	if (!transaction || !hash_buf)
	{
		fprintf(stderr, "transaction_hash: NULL parameter(s)\n");
		return (NULL);
	}
}
