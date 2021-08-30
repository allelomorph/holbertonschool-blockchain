/* -> blockchain.h -> */
/* llist.h stdint.h hblk_crypto.h */
/* iv_info_t */
#include "transaction.h"
/* fprintf */
#include <stdio.h>
/* memcmp */
#include <string.h>
/* free */
#include <stdlib.h>


/**
 * matchUnspentOut - used as `identifier` for llist_find_node to compare block,
 *   transaction and output hashes of unspent output against those recorded
 *   in an input
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
 * totalOutputAmt - used as `action` for llist_for_each to visit each
 *   output in a transaction->outputs list, and find the total coin
 *
 * @tx_out: pointer to an output in transaction->outputs list,
 *   as iterated through by llist_for_each
 * @idx: index of `tx_out` in transaction->outputs list, as
 *   iterated through by llist_for_each
 * @tl_output_amt: pointer to total output amount, modified by reference
 *
 * Return: 0 on incremental success (llist_for_each can continue,)
 *   or -2 on failure (-1 reserved for llist_for_each errors)
 */
static int totalOutputAmt(tx_out_t *tx_out, unsigned int idx,
			  uint32_t *tl_output_amt)
{
	(void)idx;

	if (!tx_out || !tl_output_amt)
	{
		fprintf(stderr, "totalOutputAmt: NULL parameter(s)\n");
		return (-2);
	}

	*tl_output_amt += tx_out->amount;
	return (0);
}


/**
 * validateTxInput - used as `action` for llist_for_each to validate each
 *   input in a transaction->inputs list. Each input in the transaction must
 *   refer to an unspent output in all_unspent, and each input’s signature
 *   must be verified using the public key stored in the referenced unspent
 *   output
 *
 * @tx_in: pointer to an input in transaction->inputs list,
 *   as iterated through by llist_for_each
 * @idx: index of `tx_in` in transaction->inputs list, as
 *   iterated through by llist_for_each
 * @iv_info: pointer to input validation info struct, containing the remaining
 *   parameters needed to perform transaction input validation
 *
 * Return: 0 on incremental success (llist_for_each can continue,)
 *   -2 on failure (-1 reserved for llist_for_each errors,)
 *   and -3 when invalid transaction input found
 */
static int validateTxInput(tx_in_t *tx_in, unsigned int idx,
			   iv_info_t *iv_info)
{
	unspent_tx_out_t *unspent_tx_out;
	EC_KEY *owner;

	(void)idx;
	if (!tx_in || !iv_info)
	{
		fprintf(stderr, "validateTxInput: NULL parameter(s)\n");
		return (-2);
	}
	unspent_tx_out = llist_find_node(iv_info->all_unspent,
					 (node_ident_t)matchUnspentOut,
					 tx_in);
	if (!unspent_tx_out)
	{
		if (llist_errno == LLIST_NODE_NOT_FOUND)
		{
			fprintf(stderr, "validateTxInput: input found %s\n",
				"with missing referenced unspent output");
			return (-3);
		}
		fprintf(stderr, "validateTxInput: llist_find_node failure\n");
		return (-2);
	}
	owner = ec_from_pub(unspent_tx_out->out.pub);
	if (!owner)
	{
		fprintf(stderr, "validateTxInput: ec_from_pub failure\n");
		return (-2);
	}
	if (!ec_verify(owner, iv_info->tx_id,
		       SHA256_DIGEST_LENGTH, &(tx_in->sig)))
	{
		fprintf(stderr, "validateTxInput: input signature does %s\n",
			"not match referenced unspent output public key");
		EC_KEY_free(owner);
		return (-3);
	}
	iv_info->tl_input_amt += unspent_tx_out->out.amount;
	EC_KEY_free(owner);
	return (0);
}


/**
 * transaction_is_valid - checks whether a transaction is valid:
 *   - The computed hash of the transaction matches the stored hash
 *   - Each input in the transaction must refer to an unspent output in
 *       all_unspent
 *   - Each input’s signature must be verified using the public key stored in
 *       the referenced unspent output
 *   - The total of input amounts must match the total of output amounts
 *
 * @transaction: points to the transaction to verify
 * @all_unspent: list of all unspent transaction outputs to date
 *
 * Return: 1 if the transaction is valid, 0 otherwise
 */
int transaction_is_valid(transaction_t const *transaction,
			 llist_t *all_unspent)
{
	iv_info_t iv_info;
	uint32_t tl_output_amt = 0;

	if (!transaction || !all_unspent)
	{
		fprintf(stderr, "transaction_is_valid: NULL parameter(s)\n");
		return (0);
	}
	if (llist_for_each(transaction->outputs, (node_func_t)totalOutputAmt,
			   &tl_output_amt) < 0)
	{
		fprintf(stderr, "transaction_is_valid: %s\n",
			"llist_for_each failure");
		return (0);
	}

	transaction_hash(transaction, iv_info.tx_id);
	if (memcmp(transaction->id, iv_info.tx_id, SHA256_DIGEST_LENGTH) != 0)
	{
		fprintf(stderr, "transaction_is_valid: %s\n",
			"transaction ID does not match hash of transaction");
		return (0);
	}

	iv_info.tl_input_amt = 0;
	iv_info.all_unspent = all_unspent;
	if (llist_for_each(transaction->inputs, (node_func_t)validateTxInput,
			   &iv_info) < 0)
	{
		fprintf(stderr,
			"transaction_is_valid: llist_for_each failure\n");
		return (0);
	}
	if (iv_info.tl_input_amt != tl_output_amt)
	{
		fprintf(stderr, "transaction_is_valid: total of input %s\n",
			"amounts does not match total of output amounts");
		return (0);
	}
	return (1);
}
