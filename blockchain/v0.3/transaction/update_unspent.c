/* -> blockchain.h -> */
/* llist.h stdint.h hblk_crypto.h */
#include "transaction.h"
/* fprintf */
#include <stdio.h>
/* free */
#include <stdlib.h>
/* memcmp */
#include <string.h>


/**
 * matchUnspentOut - used as `identifier` for llist_find_node or
 *   llist_remove_node to compare block, transaction and output hashes of
 *   unspent output against those recorded in an input
 *
 * @unspent_tx_out: unspent output in a blockchain->unspent list, as
 *   iterated through by llist_find_node
 * @tx_in: pointer to input containing references to match
 *
 * Return: 1 if transaction hash matches `tx_in`, 0 if not or on failure
 */
static int matchUnspentOut(unspent_tx_out_t *unspent_tx_out, tx_in_t *tx_in)
{
	int match;

	if (!unspent_tx_out || !tx_in)
	{
		fprintf(stderr, "matchUnspentOut: NULL parameter(s)\n");
		return (0);
	}

	match = !(memcmp(unspent_tx_out->block_hash, tx_in->block_hash,
			 SHA256_DIGEST_LENGTH) ||
		  memcmp(unspent_tx_out->tx_id, tx_in->tx_id,
			 SHA256_DIGEST_LENGTH) ||
		  memcmp(unspent_tx_out->out.hash, tx_in->tx_out_hash,
			 SHA256_DIGEST_LENGTH));

	return (match);
}


/**
 * addUnspentOutput - used as `action` for llist_for_each to add a transaction
 *   output to the list of unspent outputs
 *
 * @tx_out: pointer to output to include
 * @idx: index of tx_out in a transaction->outputs list
 * @uo_info: pointer to struct containing parameters necessary for updating
 *   unspent outputs
 *
 * Return: 0 in incremental success (llist_for_each can continue,)
 *   and -2 on failure (-1 reserved for llist_for_each errors)
 */
static int addUnspentOutput(tx_out_t *tx_out, unsigned int idx,
			    uo_info_t *uo_info)
{
	unspent_tx_out_t *unspent_tx_out;

	(void)idx;

	if (!tx_out || !uo_info)
	{
		fprintf(stderr,
			"addUnspentOutput: NULL parameter(s)\n");
		return (-2);
	}

	unspent_tx_out = unspent_tx_out_create(uo_info->block_hash,
					       uo_info->tx_id, tx_out);
	if (!unspent_tx_out)
	{
		fprintf(stderr,
			"addUnspentOutput: unspent_tx_out_create failure \n");
		return (-2);
	}

	if (llist_add_node(uo_info->all_unspent, unspent_tx_out,
			   ADD_NODE_REAR) < 0)
	{
		fprintf(stderr, "addUnspentOutput: llist_add_node failure\n");
		return (-2);
	}

	return (0);
}


/**
 * delRfrncdOutput - used as `action` for llist_for_each to delete the unspent
 *   output referenced by a given transaction input
 *
 * @tx_in: pointer to input containing references to match
 * @idx: index of tx_in in a transaction->inputs list
 * @unspent: list of all unspent outputs to be amended
 *
 * Return: 0 in incremental success (llist_for_each can continue,)
 *   and -2 on failure (-1 reserved for llist_for_each errors)
 */
int delRfrncdOutput(tx_in_t *tx_in, unsigned int idx,
		    llist_t *unspent)
{
	uint8_t *hash;
	int i, end, hashes_zeroed;

	(void)idx;

	if (!tx_in || !unspent)
	{
		fprintf(stderr,
			"delRfrncdOutput: NULL parameter(s)\n");
		return (-2);
	}

	/* tx_in->block_hash and tx_in->tx_id */
	hash = tx_in->block_hash;
	end = SHA256_DIGEST_LENGTH * 2;
	for (i = 0, hashes_zeroed = 1; i < end; i++)
		hashes_zeroed &= (hash[i] == 0);

	/* coinbase tx inputs by definition have no referenced outputs */
	if (hashes_zeroed)
		return (0);

	if (llist_remove_node(unspent, (node_ident_t)matchUnspentOut,
			      tx_in, 1, NULL) < 0)
	{
		fprintf(stderr, "delRfrncdOutput: llist_remove_node: %s\n",
			strE_LLIST(llist_errno));
		return (-2);
	}

	return (0);
}


/**
 * updateRfrncdOutputs - used as `action` for llist_for_each to udpate the
 *   unspent output list for a given transaction:
 *   - delete the unspent outputs referenced by its inputs
 *   - add the transaction's outputs as new unspent outputs in the list
 *
 * @tx: pointer to transaction to scan
 * @idx: index of tx in a block->transactions list
 * @uo_info: pointer to struct containing parameters necessary for updating
 *   unspent outputs
 *
 * Return: 0 in incremental success (llist_for_each can continue,)
 *   and -2 on failure (-1 reserved for llist_for_each errors)
 */
static int updateRfrncdOutputs(transaction_t *tx, unsigned int idx,
			       uo_info_t *uo_info)
{
	(void)idx;

	if (!tx || !uo_info)
	{
		fprintf(stderr,
			"updateRfrncdOutputs: NULL parameter(s)\n");
		return (-2);
	}

	memcpy(&(uo_info->tx_id), &(tx->id), SHA256_DIGEST_LENGTH);

	if (llist_for_each(tx->inputs, (node_func_t)delRfrncdOutput,
			   uo_info->all_unspent) < 0 ||
	    llist_for_each(tx->outputs, (node_func_t)addUnspentOutput,
			   uo_info) < 0)
	{
		fprintf(stderr,
			"updateRfrncdOutputs: llist_for_each failure\n");
		return (-2);
	}

	return (0);
}


/**
 * update_unspent - updates the list of all unspent transaction outputs, given
 *   a list of processed transactions
 *
 * @transactions: list of validated transactions
 * @block_hash: hash of the validated block that contains `transactions`
 * @all_unspent: current list of unspent transaction outputs
 *
 * Return: pointer to new list of unspent transaction outputs upon success,
 *   or NULL upon failure
 */
llist_t *update_unspent(llist_t *transactions,
			uint8_t block_hash[SHA256_DIGEST_LENGTH],
			llist_t *all_unspent)
{
	uo_info_t uo_info;

	if (!transactions || !block_hash || !all_unspent)
	{
		fprintf(stderr, "update_unspent: NULL parameter(s)\n");
		return (NULL);
	}

	/*
	 * for every tx in transactions:
	 *    for every input in tx->inputs:
	 *        match input hashes with unspent output in all_unspent
	 *	  delete unspent output with free
	 *    for every output in tx->outputs:
	 *        add output to all_unspent
	 */

	memcpy(&(uo_info.block_hash), block_hash, SHA256_DIGEST_LENGTH);
	uo_info.all_unspent = all_unspent;

	if (llist_for_each(transactions, (node_func_t)updateRfrncdOutputs,
			   &uo_info) < 0)
	{
		fprintf(stderr, "update_unspent: llist_for_each failure\n");
		return (NULL);
	}

	return (all_unspent);
}
