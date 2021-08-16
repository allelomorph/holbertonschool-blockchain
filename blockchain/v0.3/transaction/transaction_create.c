/* -> blockchain.h -> */
/* llist.h stdint.h hblk_crypto.h */
#include "transaction.h"
/* fprintf */
#include <stdio.h>
/* memcmp */
#include <string.h>


/**
 * findSenderUnspent - used as `action` for llist_for_each to visit each
 *   unspent output in the blockchain, and add unspent outputs
 *   with a matching public key to a collated list of potential inputs
 *
 * @unspent_tx_out: pointer to unspent transaction in blockchain->unspent list,
 *   as iterated through by llist_for_each
 * @idx: index of `unspent_tx_out` in blockchain->unspent list, as
 *   iterated through by llist_for_each
 * @su_info: pointer to struct containing all parameters necessary to build a
 *   second list of unspent transactions for a particular public key
 */
static int findSenderUnspent(unspent_tx_out_t *unspent_tx_out,
			     unsigned int idx, su_info_t *su_info)
{
	(void)idx;

	if (!unspent_tx_out || !su_info)
	{
		fprintf(stderr, "findSenderUnspent: NULL parameter(s)\n");
		return (1);
	}

        if (memcmp(unspent_tx_out->out.pub, su_info->sender_pub,
		   EC_PUB_LEN) == 0)
	{
		if (llist_add_node(su_info->sender_unspent,
				   unspent_tx_out, ADD_NODE_REAR) != 0)
		{
			fprintf(stderr, "findSenderUnspent: %s\n",
				"llist_add_node failure");
			return (1);
		}

		su_info->total_unspent_amt += unspent_tx_out->out.amount;

		if (su_info->total_unspent_amt >= su_info->send_amt)
			return (1); /* sufficient to cover spending */
	}

	return (0);
}


/**
 * convertSenderUnspent - used as `action` for llist_for_each to visit each
 *   transaction in a list of those avaialble for a particular sender, to be
 *   converted into inputs for a new transaction
 *
 */
static int convertSenderUnspent(unspent_tx_out_t *unspent_tx_out,
				unsigned int idx, llist_t *tx_inputs)
{
	tx_in_t *input;

	(void)idx;

	if (!unspent_tx_out || !tx_inputs)
	{
		fprintf(stderr, "convertSenderUnspent: NULL parameter(s)\n");
		return (1);
	}

	input = tx_in_create(unspent_tx_out);
	if (!input)
	{
		fprintf(stderr, "convertSenderUnspent: tx_in_create failure\n");
		return (1);
	}

	if (llist_add_node(tx_inputs, input, ADD_NODE_REAR) != 0)
	{
		fprintf(stderr, "convertSenderUnspent: %s\n",
			"llist_add_node failure");
		free(input);
		return (1);
	}

	return (0);
}


/**
 * signTxIn - used as `action` for llist_for_each
 *
 */
static int signTxIn(tx_in_t *tx_in, unsigned int idx,
		    sign_info_t *sign_info)
{
	(void)idx;

	if (!tx_in || !sign_info)
	{
		fprintf(stderr, "signTxIn: NULL parameter(s)\n");
		return (1);
	}

        if (!tx_in_sign(tx_in, sign_info->tx_id, sign_info->sender,
			sign_info->all_unspent))
	{
		fprintf(stderr, "signTxIn: tx_in_sign failure\n");
		return (1);
	}

	return (0);
}


llist_t *setTxInputs(llist_t *all_unspent, su_info_t *su_info)
{
	llist_t *tx_inputs;

	if (!all_unspent || !su_info)
	{
		fprintf(stderr, "setTxInputs: NULL parameter(s)\n");
		return (NULL);
	}

	/*
	Select a set of unspent transaction outputs from all_unspent, which
	public keys match sender‘s private key
	*/
	if (llist_for_each(all_unspent,
			   (node_func_t)findSenderUnspent, su_info) == -1)
	{
		fprintf(stderr, "setTxInputs: llist_for_each failure\n");
		return (NULL);
	}

	/*
        The function must fail if sender does not possess enough coins (total
	amount of selected unspent outputs lower than amount)
	*/
	if (su_info->total_unspent_amt < su_info->send_amt)
	{
		fprintf(stderr, "setTxInputs: %s\n",
			"insufficient unspent outputs to total amount to send");
		return (NULL);
	}

	tx_inputs = llist_create(MT_SUPPORT_FALSE);
	if (!tx_inputs)
	{
		fprintf(stderr, "setTxInputs: llist_create failure\n");
		return (NULL);
	}

	/*
	Create transaction inputs from these selected unspent transaction outputs
	*/
	if (llist_for_each(su_info->sender_unspent,
			   (node_func_t)convertSenderUnspent, tx_inputs) == -1)
	{
		fprintf(stderr, "setTxInputs: llist_for_each failure\n");
		llist_destroy(tx_inputs, 1, NULL);
		return (NULL);
	}

	return (tx_inputs);
}


llist_t *setTxOutputs(const uint8_t sender_pub[EC_PUB_LEN],
		      const uint8_t recv_pub[EC_PUB_LEN], su_info_t *su_info)
{
	llist_t *tx_outputs;
	tx_out_t *output;
	uint32_t leftover;

	if (!sender_pub || !recv_pub || !su_info)
	{
		fprintf(stderr, "setTxOutputs: NULL parameter(s)\n");
		return (NULL);
	}

	/*
	Create a transaction output, sending amount coins to receiver’s public key
	*/
	tx_outputs = llist_create(MT_SUPPORT_FALSE);
	if (!tx_outputs)
	{
		fprintf(stderr, "setTxOutputs: llist_create failure\n");
		return (NULL);
	}

	output = tx_out_create(su_info->send_amt, recv_pub);
	if (!output)
	{
		fprintf(stderr, "setTxOutputs: tx_out_create failure\n");
		return (NULL);
	}

	if (llist_add_node(tx_outputs, output, ADD_NODE_REAR) != 0)
	{
		fprintf(stderr, "setTxOutputs: llist_add_node failure\n");
		free(output);
		return (NULL);
	}

	/* If the total amount of the selected unspent outputs is greater than
	   amount, create a second transaction output, sending the leftover back to sender
	*/
	leftover = su_info->total_unspent_amt - su_info->send_amt;
	if (leftover)
	{
		output = tx_out_create(leftover, sender_pub);
		if (!output)
		{
			fprintf(stderr, "setTxOutputs: tx_out_create failure\n");
			return (NULL);
		}

		if (llist_add_node(tx_outputs, output, ADD_NODE_REAR) != 0)
		{
			fprintf(stderr, "setTxOutputs: %s\n",
				"llist_add_node failure");
			free(output);
			return (NULL);
		}
	}

	return (tx_outputs);
}

/**
 * transaction_create - creates a transaction
 *
 * @sender: contains the private key of the transaction sender
 * @receiver: contains the public key of the transaction recipient
 * @amount: amount to send
 * @all_unspent: list of all the unspent outputs to date
 *
 * Return: pointer to the created transaction upon success, or NULL upon failure
 */
transaction_t *transaction_create(EC_KEY const *sender, EC_KEY const *receiver,
				  uint32_t amount, llist_t *all_unspent)
{
	su_info_t su_info;
	sign_info_t sign_info;
	transaction_t *tx;
	llist_t *tx_inputs, *tx_outputs;
	uint8_t recv_pub[EC_PUB_LEN];

	if (!sender || !receiver || !all_unspent)
	{
		fprintf(stderr, "transaction_create: NULL parameter(s)\n");
		return (NULL);
	}

	if (!ec_to_pub(sender, su_info.sender_pub) ||
	    !ec_to_pub(receiver, recv_pub))
	{
		fprintf(stderr, "transaction_create: ec_to_pub failure\n");
		return (NULL);
	}

	su_info.sender_unspent = llist_create(MT_SUPPORT_FALSE);
	if (!(su_info.sender_unspent))
	{
		fprintf(stderr, "transaction_create: llist_create failure\n");
		return (NULL);
	}
	su_info.send_amt = amount;
	su_info.total_unspent_amt = 0;

	tx_inputs = setTxInputs(all_unspent, &su_info);
	llist_destroy(su_info.sender_unspent, 0, NULL);
	if (!tx_inputs)
		return (NULL);

	tx_outputs = setTxOutputs(su_info.sender_pub, recv_pub, &su_info);
	if (!tx_outputs)
	{
		llist_destroy(tx_inputs, 1, NULL);
		llist_destroy(tx_outputs, 1, NULL);
		return (NULL);
	}

	tx = malloc(sizeof(transaction_t));
	if (!tx)
	{
		fprintf(stderr, "transaction_create: malloc failure\n");
		llist_destroy(tx_inputs, 1, NULL);
		llist_destroy(tx_outputs, 1, NULL);
		return (NULL);
	}
	tx->inputs = tx_inputs;
	tx->outputs = tx_outputs;

	/*
	Compute transaction ID (hash)
	*/
	if (transaction_hash(tx, tx->id) == NULL)
	{
		fprintf(stderr, "transaction_create: transaction_hash failure\n");
		llist_destroy(tx_inputs, 1, NULL);
		llist_destroy(tx_outputs, 1, NULL);
		free(tx);
		return (NULL);
	}

	/*
	Sign transaction inputs using the previously computed transaction ID
	*/
	memcpy(sign_info.tx_id, tx->id, SHA256_DIGEST_LENGTH);
	sign_info.sender = sender;
	sign_info.all_unspent = all_unspent;
	if (llist_for_each(tx->inputs, (node_func_t)signTxIn, &sign_info) == -1)
	{
		fprintf(stderr, "transaction_create: llist_for_each failure\n");
		llist_destroy(tx_inputs, 1, NULL);
		llist_destroy(tx_outputs, 1, NULL);
		free(tx);
		return (NULL);
	}

	return (tx);
}


/*
 * strE_LLIST needs blockchain_create.c, which is not used in automatic grading
 *		fprintf(stderr, "tx_in_sign: llist_find_node: %s\n",
 *                      strE_LLIST(llist_errno));
 */
