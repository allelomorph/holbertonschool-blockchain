/* MEMPOOL_PATH_DFLT */
#include "hblk_cli.h"
/* printf fprintf */
#include <stdio.h>
/* strerror memcmp */
#include <string.h>
/* isdigit */
#include <ctype.h>
/* atoi */
#include <stdlib.h>


/**
 * findSenderUnspent - used as `action` for llist_for_each to visit each
 *   unspent output in the blockchain, and add unspent outputs
 *   with a matching public key to a collated list of potential inputs
 *
 * @unspent_tx_out: pointer to unspent output in blockchain->unspent list,
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
 * pubKeyHexToByteArray - converts a EC_KEY public key in a lowercase or
 *   uppercase hex string to a byte array
 *
 * @address: public key of EC_PUB_LEN bytes, in hex
 *
 * Return: byte array on success, or NULL on failure
 */
uint8_t *pubKeyHexToByteArray(char *address)
{
	uint8_t *pub;
	unsigned int scan;
	size_t i, j, len, upper, case_detected;

	if (!address)
	{
		fprintf(stderr, "pubKeyHexToByteArray: NULL parameter\n");
		return (NULL);
	}

	pub = calloc(EC_PUB_LEN, sizeof(uint8_t));
	if (!pub)
	{
		fprintf(stderr, "pubKeyHexToByteArray: calloc failure\n");
		return (NULL);
	}

	len = strlen(address);
	if (len < (EC_PUB_LEN * 2))
	{
		free(pub);
		return (NULL);
	}

	for (i = 0, case_detected = 0; i < len; i++)
	{
		if (isdigit(address[i]))
			continue;
		else if (address[i] >= 'a' && address[i] <= 'f')
		{
			if (!case_detected)
			{
				upper = 0;
				case_detected = 1;
			}
			if (upper)
			{
				free(pub);
				return (NULL);
			}
		}
		else if (address[i] >= 'A' && address[i] <= 'F')
		{
			if (!case_detected)
			{
				upper = 1;
				case_detected = 1;
			}
			if (!upper)
			{
				free(pub);
				return (NULL);
			}
		}
		else
		{
			free(pub);
			return (NULL);
		}

	}

	for (i = 0, j = 0; i < EC_PUB_LEN; i++, j += 2)
	{
		if (sscanf(address + j, upper ? "%02X" : "%02x", &scan) == -1)
		{
			perror("pubKeyHexToByteArray: sscanf");
			free(pub);
			return (NULL);
		}
		pub[i] = scan;
	}

	return (pub);
}


/**
 * cmd_send - initiates a transaction with the current wallet sending `amount`
 *   coin to `address`, adding it to the session mempool
 *
 * @amount: user provided amount of coin to send; expecting string representing
 *   positive integer
 * @address: public key of recipient, in hex
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 *
 * Return: 0 on success, 1 on failure
 */
int cmd_send(char *amount, char *address, cli_state_t *cli_state)
{
	EC_KEY *receiver;
	transaction_t *tx;
	uint8_t *pub;
	su_info_t su_info;
	int amt;
	size_t i;

	if (!cli_state)
	{
		fprintf(stderr, "cmd_send: NULL cli_state parameter\n");
		return (1);
	}

	if (!amount || !address)
	{
		printf(TAB4 "Usage: send <amount> <address>\n");
		return (1);
	}

	for (i = 0; i < strlen(amount); i++)
	{
		if (!isdigit(amount[i]))
		{
			printf(TAB4 TAB4 "Transaction amount must be a positive integer\n");
			return (1);
		}
	}
	amt = atoi(amount);

	su_info.sender_unspent = llist_create(MT_SUPPORT_FALSE);
	if (!su_info.sender_unspent)
	{
		fprintf(stderr, "cmd_send: llist_create failure\n");
		return (1);
	}
	if (!ec_to_pub(cli_state->wallet, su_info.sender_pub))
	{
		fprintf(stderr, "cmd_send: ec_to_pub failure\n");
		return (1);
	}
	su_info.send_amt = (uint32_t)amt;
	su_info.total_unspent_amt = 0;
	if (llist_for_each(cli_state->unspent_cache,
			   (node_func_t)findSenderUnspent, &su_info) < 0)
	{
		fprintf(stderr, "cmd_send: llist_for_each failure\n");
		return (1);
	}
	llist_destroy(su_info.sender_unspent, 0, NULL);
	if (su_info.total_unspent_amt < (uint32_t)amt)
	{
		printf(TAB4 TAB4 "%s total %u insufficient to send %i\n",
		       "Wallet confirmed unspent output",
		       su_info.total_unspent_amt, amt);
		return (1);
	}

	pub = pubKeyHexToByteArray(address);
	if (!pub)
	{
		printf(TAB4 TAB4 "Expecting address to be %s %i bytes\n",
		       "a hex string representing", EC_PUB_LEN);
		return (1);
	}

	receiver = ec_from_pub(pub);
	free(pub);
	if (!receiver)
	{
		printf(TAB4 "Failed to create reciever's key with address\n");
		return (1);
	}

	tx = transaction_create(cli_state->wallet, receiver, amt,
				cli_state->unspent_cache);
	EC_KEY_free(receiver);
	if (!tx || !transaction_is_valid(tx, cli_state->unspent_cache) ||
	    llist_for_each(tx->inputs, (node_func_t)delRfrncdOutput,
			   cli_state->unspent_cache) < 0)
	{
		if (tx)
			transaction_destroy(tx);
		printf(TAB4 "Failed to create transaction\n");
		return (1);
	}

	if (llist_add_node(cli_state->mempool, tx, ADD_NODE_REAR) == -1)
	{
		transaction_destroy(tx);
		printf(TAB4 "Failed to add transaction to mempool\n");
		return (1);
	}

	printf(TAB4 "%s, sending %s to provided public key\n",
	       "Verified transaction added to mempool", amount);
	return (0);
}
