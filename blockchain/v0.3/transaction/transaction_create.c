/* -> blockchain.h -> */
/* llist.h stdint.h hblk_crypto.h */
/* su_info_t tx_in_create tx_in_sign sign_info tx_out_create transaction_hash*/
#include "transaction.h"
/* fprintf */
#include <stdio.h>
/* memcmp */
#include <string.h>
/* malloc free */
#include <stdlib.h>


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
 *
 * Return: 0 on incremental success (llist_for_each can continue,)
 *   1 on total success (llist_for_each can end,)
 *   and -2 on failure (-1 reserved for llist_for_each errors)
 */
static int findSenderUnspent(unspent_tx_out_t *unspent_tx_out,
			     unsigned int idx, su_info_t *su_info)
{
	(void)idx;

	if (!unspent_tx_out || !su_info)
	{
		fprintf(stderr, "findSenderUnspent: NULL parameter(s)\n");
		return (-2);
	}

	if (memcmp(unspent_tx_out->out.pub, su_info->sender_pub,
		   EC_PUB_LEN) == 0)
	{
		if (llist_add_node(su_info->sender_unspent,
				   unspent_tx_out, ADD_NODE_REAR) != 0)
		{
			fprintf(stderr, "findSenderUnspent: %s\n",
				"llist_add_node failure");
			return (-2);
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
 * @unspent_tx_out: pointer to unspent transaction in blockchain->unspent list,
 *   as iterated through by llist_for_each
 * @idx: index of `unspent_tx_out` in blockchain->unspent list, as
 *   iterated through by llist_for_each
 * @tx_inputs: pointer to list of inputs generated from unspent outputs
 *
 * Return: 0 on success, 1 on failure
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
 * signTxIn - used as `action` for llist_for_each, to sign all the inpputs for
 *   a given transaction
 *
 * @tx_in: pointer to input in a transaction->inputs list, as iterated through
 *   by llist_for_each
 * @idx: index of `tx_in` in transaction->inputs list, as iterated through by
 *   llist_for_each
 * @sign_info: pointer to struct containing all other parameters necessary to
 *   sign a transaction input
 *
 * Return: 0 on success, 1 on failure
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


/**
 * setTxInputs - populates list of valid transaction inputs: checks list of
 *   all unspent for the sender's unspent outputs, verifies that there is a
 *   positive balance in excess of the new transaction amount, and converts
 *   those unspent outputs to a list of transaction inputs.
 *
 * @all_unspent: list of all the unspent outputs to date
 * @su_info:  pointer to "sender unspent" info struct, containing parameters
 *   used and populated during the collation of a list of the sender's unspent
 *   outputs
 *
 * Return: 0 on success, 1 on failure
 */
llist_t *setTxInputs(llist_t *all_unspent, su_info_t *su_info)
{
	llist_t *tx_inputs;

	if (!all_unspent || !su_info)
	{
		fprintf(stderr, "setTxInputs: NULL parameter(s)\n");
		return (NULL);
	}

	if (llist_for_each(all_unspent,
			   (node_func_t)findSenderUnspent, su_info) < 0)
	{
		fprintf(stderr, "setTxInputs: llist_for_each failure\n");
		return (NULL);
	}

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

	if (llist_for_each(su_info->sender_unspent,
			   (node_func_t)convertSenderUnspent, tx_inputs) != 0)
	{
		fprintf(stderr, "setTxInputs: llist_for_each failure\n");
		llist_destroy(tx_inputs, 1, NULL);
		return (NULL);
	}

	return (tx_inputs);
}


/**
 * setTxOutputs - populates list of outputs for a new transaction: creates
 *   output to recipient in the amount requested by sender, and if necessary a
 *   second output sending the excess back to the sender.
 *
 * @sender_pub: sender's public key
 * @recv_pub: recipient's public key
 * @su_info: pointer to "sender unspent" info struct, containing other
 *   parameters necessary to calculate the second output
 *
 * Return: 0 on success, 1 on failure
 */
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

	output = tx_out_create(su_info->send_amt, recv_pub);
	if (!output)
	{
		fprintf(stderr, "setTxOutputs: tx_out_create failure\n");
		return (NULL);
	}
	tx_outputs = llist_create(MT_SUPPORT_FALSE);
	if (!tx_outputs)
	{
		fprintf(stderr, "setTxOutputs: llist_create failure\n");
		free(output);
		return (NULL);
	}
	if (llist_add_node(tx_outputs, output, ADD_NODE_REAR) != 0)
	{
		fprintf(stderr, "setTxOutputs: llist_add_node failure\n");
		free(output);
		llist_destroy(tx_outputs, 1, NULL);
		return (NULL);
	}

	leftover = su_info->total_unspent_amt - su_info->send_amt;
	if (leftover)
	{
		output = tx_out_create(leftover, sender_pub);
		if (!output)
		{
			fprintf(stderr, "setTxOutputs: %s\n",
				"tx_out_create failure");
			llist_destroy(tx_outputs, 1, NULL);
			return (NULL);
		}

		if (llist_add_node(tx_outputs, output, ADD_NODE_REAR) != 0)
		{
			fprintf(stderr, "setTxOutputs: %s\n",
				"llist_add_node failure");
			free(output);
			llist_destroy(tx_outputs, 1, NULL);
			return (NULL);
		}
	}

	return (tx_outputs);
}


/**
 * newTransaction - creates a new transaction from lists of inputs and outputs
 *
 * @tx_inputs: list of transaction inputs
 * @tx_outputs: list of transaction outputs
 * @sender: contains the private key of the transaction sender
 * @all_unspent: list of all the unspent outputs to date
 *
 * Return: pointer to the created transaction upon success, or NULL upon
 *   failure
 */
transaction_t *newTransaction(llist_t *tx_inputs, llist_t *tx_outputs,
			      const EC_KEY *sender, llist_t *all_unspent)
{
	transaction_t *tx;
	sign_info_t sign_info;

	if (!tx_inputs || !tx_outputs)
	{
		fprintf(stderr, "newTransaction: NULL parameter(s)\n");
		return (NULL);
	}

	tx = malloc(sizeof(transaction_t));
	if (!tx)
	{
		fprintf(stderr, "newTransaction: malloc failure\n");
		return (NULL);
	}
	tx->inputs = tx_inputs;
	tx->outputs = tx_outputs;

	if (transaction_hash(tx, tx->id) == NULL)
	{
		fprintf(stderr, "newTransaction: transaction_hash failure\n");
		free(tx);
		return (NULL);
	}

	memcpy(sign_info.tx_id, tx->id, SHA256_DIGEST_LENGTH);
	sign_info.sender = sender;
	sign_info.all_unspent = all_unspent;
	if (llist_for_each(tx->inputs, (node_func_t)signTxIn, &sign_info) != 0)
	{
		fprintf(stderr, "newTransaction: llist_for_each failure\n");
		free(tx);
		return (NULL);
	}

	return (tx);
}


/**
 * transaction_create - creates a transaction
 *
 * @sender: contains the private key of the transaction sender
 * @receiver: contains the public key of the transaction recipient
 * @amount: amount to send
 * @all_unspent: list of all the unspent outputs to date
 *
 * Return: pointer to the created transaction upon success, or NULL upon
 *   failure
 */
transaction_t *transaction_create(EC_KEY const *sender, EC_KEY const *receiver,
				  uint32_t amount, llist_t *all_unspent)
{
	su_info_t su_info;
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
		return (NULL);
	}

	tx = newTransaction(tx_inputs, tx_outputs, sender, all_unspent);
	if (!tx)
	{
		llist_destroy(tx_inputs, 1, NULL);
		llist_destroy(tx_outputs, 1, NULL);
		return (NULL);
	}

	return (tx);
}


/*
 * strE_LLIST needs blockchain_create.c, which is not used in automatic grading
 *		fprintf(stderr, "tx_in_sign: llist_find_node: %s\n",
 *                      strE_LLIST(llist_errno));
 */
