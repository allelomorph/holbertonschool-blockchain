/* -> blockchain.h -> */
/* llist.h stdint.h hblk_crypto.h */
#include "transaction.h"
/* fprintf */
#include <stdio.h>
/* memcpy bzero */
#include <string.h>
/* free */
#include <stdlib.h>


/**
 * coinbaseTxCleanup - frees memory allocated by coinbase_create when failure
 *   condition met
 *
 * @cb_input: pointer to single input of coinbase transaction
 * @cb_output: pointer to single output of coinbase transaction
 * @cb_tx: pointer to coinbase transaction
 */
void coinbaseTxCleanup(tx_in_t *cb_input, tx_out_t *cb_output,
		       transaction_t *cb_tx)
{
	if (cb_input)
		free(cb_input);

	if (cb_output)
		free(cb_output);

	if (cb_tx)
	{
		llist_destroy(cb_tx->inputs, 0, NULL);
		llist_destroy(cb_tx->outputs, 0, NULL);
		free(cb_tx);
	}
}


/**
 * newCoinbaseTx - generates coinbase transaction from input and output
 *
 * @cb_input: pointer to single input of coinbase transaction
 * @cb_output: pointer to single output of coinbase transaction
 *
 * Return: pointer to the created transaction upon success, or NULL upon
 *   failure
 */
transaction_t *newCoinbaseTx(tx_in_t *cb_input, tx_out_t *cb_output)
{
	transaction_t *cb_tx;

	if (!cb_input || !cb_output)
	{
		fprintf(stderr, "newCoinbaseTx: NULL parameter(s)\n");
		return (NULL);
	}

	cb_tx = malloc(sizeof(transaction_t));
	if (!cb_tx)
	{
		fprintf(stderr, "newCoinbaseTx: malloc failure\n");
		coinbaseTxCleanup(cb_input, cb_output, cb_tx);
		return (NULL);
	}

	cb_tx->inputs = llist_create(MT_SUPPORT_FALSE);
	cb_tx->outputs = llist_create(MT_SUPPORT_FALSE);
	if (!cb_tx->inputs || !cb_tx->outputs)
	{
		fprintf(stderr, "newCoinbaseTx: llist_create failure\n");
		coinbaseTxCleanup(cb_input, cb_output, cb_tx);
		return (NULL);
	}

	if (llist_add_node(cb_tx->inputs, cb_input, ADD_NODE_REAR == -1) ||
	    llist_add_node(cb_tx->outputs, cb_output, ADD_NODE_REAR == -1))
	{
		fprintf(stderr, "newCoinbaseTx: llist_create failure\n");
		coinbaseTxCleanup(cb_input, cb_output, cb_tx);
		return (NULL);
	}

	if (!transaction_hash(cb_tx, cb_tx->id))
	{
		fprintf(stderr, "newCoinbaseTx: transaction_hash failure\n");
		coinbaseTxCleanup(cb_input, cb_output, cb_tx);
		return (NULL);
	}
	return (cb_tx);
}


/**
 * coinbase_create - creates a coinbase transaction
 *
 * @receiver: contains the public key of the miner, who will receive the
 *   coinbase coins
 * @block_index: index of the block the coinbase transaction will belong to
 *
 * Return: pointer to the created transaction upon success, or NULL upon
 *   failure
 */
transaction_t *coinbase_create(EC_KEY const *receiver, uint32_t block_index)
{
	uint8_t recv_pub[EC_PUB_LEN];
	tx_in_t *cb_input;
	tx_out_t *cb_output;
	transaction_t *cb_tx;

	if (!receiver)
	{
		fprintf(stderr, "coinbase_create: NULL parameter\n");
		return (NULL);
	}
	if (!ec_to_pub(receiver, recv_pub))
	{
		fprintf(stderr, "coinbase_create: ec_to_pub failure\n");
		return (NULL);
	}

	cb_input = malloc(sizeof(tx_in_t));
	if (!cb_input)
	{
		fprintf(stderr, "coinbase_create: malloc failure\n");
		return (NULL);
	}
	bzero(cb_input, sizeof(tx_in_t));
	memcpy(&(cb_input->tx_out_hash), &(block_index), sizeof(uint32_t));

	cb_output = tx_out_create(COINBASE_AMOUNT, recv_pub);
	if (!cb_output)
	{
		fprintf(stderr, "coinbase_create: tx_out_create failure\n");
		free(cb_input);
		return (NULL);
	}

	cb_tx = newCoinbaseTx(cb_input, cb_output);
	if (!cb_tx)
	{
		fprintf(stderr, "coinbase_create: newCoinbaseTx failure\n");
		return (NULL);
	}
	return (cb_tx);
}
