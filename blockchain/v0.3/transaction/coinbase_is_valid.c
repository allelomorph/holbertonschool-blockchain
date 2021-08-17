/* -> blockchain.h -> */
/* llist.h stdint.h hblk_crypto.h */
#include "transaction.h"
/* fprintf */
#include <stdio.h>
/* memcpy */
#include <string.h>
/* free */
#include <stdlib.h>


/**
 * coinbase_is_valid - checks whether a coinbase transaction is valid:
 *   - The computed hash of the transaction must match the stored hash
 *   - The transaction contains exactly 1 input
 *   - The transaction contains exactly 1 output
 *   - The first 4 bytes of the transaction input’s tx_out_hash must match the
 *       block_index
 *   - The transaction input’s block_hash, tx_id, and signature must be zeroed
 *   - The transaction output amount must be exactly COINBASE_AMOUNT
 *
 * @coinbase: points to the coinbase transaction to verify
 * @block_index: index of the block the coinbase transaction will belong to
 *
 * Return: 1 if the coinbase transaction is valid, and 0 otherwise
 */
int coinbase_is_valid(transaction_t const *coinbase, uint32_t block_index)
{
	uint8_t cb_hash[SHA256_DIGEST_LENGTH], *hash;
	tx_in_t *cb_input;
	tx_out_t *cb_output;
	int i, end, zeroed = 1;

	if (!coinbase)
	{
		fprintf(stderr, "coinbase_is_valid: NULL parameter\n");
		return (0);
	}

	if (!transaction_hash(coinbase, cb_hash))
	{
		fprintf(stderr, "coinbase_is_valid: %s\n",
			"transaction_hash failure");
		return (0);
	}
	if (memcmp(coinbase->id, cb_hash, SHA256_DIGEST_LENGTH) != 0)
	{
		fprintf(stderr, "coinbase_is_valid: %s\n",
			"hash of transaction does not match id");
		return (0);
	}

	if (llist_size(coinbase->inputs) != 1)
	{
		fprintf(stderr, "coinbase_is_valid: %s\n",
			"invalid number of inputs");
		return (0);
	}
	if (llist_size(coinbase->outputs) != 1)
	{
		fprintf(stderr, "coinbase_is_valid: %s\n",
			"invalid number of outputs");
		return (0);
	}

	cb_input = llist_get_head(coinbase->inputs);
	if (!cb_input)
	{
		fprintf(stderr, "coinbase_is_valid: %s\n",
			"llist_get_head failure");
		return (0);
	}

	if (memcmp(&(cb_input->tx_out_hash), &(block_index),
		   sizeof(uint32_t)) != 0)
	{
		fprintf(stderr, "coinbase_is_valid: first %lu bytes %s\n",
			sizeof(uint32_t),
			"of input tx_out_hash do not match block_index");
		return (0);
	}

	hash = cb_input->block_hash;
	end = SHA256_DIGEST_LENGTH * 2;
	for (i = 0; i < end; i++)
		zeroed = (hash[i] == 0);
	hash = cb_input->tx_out_hash + sizeof(uint32_t);
	end = SHA256_DIGEST_LENGTH - sizeof(uint32_t);
	for (i = 0; i < end; i++)
		zeroed = (hash[i] == 0);
	if (!zeroed)
	{
		fprintf(stderr, "coinbase_is_valid: input block_hash, %s\n",
			"tx_id, and tx_out_hash not all properly zeroed");
		return (0);
	}

	cb_output = llist_get_head(coinbase->outputs);
	if (!cb_output)
	{
		fprintf(stderr, "coinbase_is_valid: %s\n",
			"llist_get_head failure");
		return (0);
	}

	if (cb_output->amount != COINBASE_AMOUNT)
	{
		fprintf(stderr, "coinbase_is_valid: %s\n",
			"output amount does not match expected value");
		return (0);
	}

	return (1);
/*
    The computed hash of the transaction must match the hash stored in it
    The transaction must contain exactly 1 input
    The transaction must contain exactly 1 output
    The transaction input’s tx_out_hash first 4 bytes must match the block_index
    The transaction input’s block_hash, tx_id, and signature must be zeroed
    The transaction output amount must be exactly COINBASE_AMOUNT
*/
}
