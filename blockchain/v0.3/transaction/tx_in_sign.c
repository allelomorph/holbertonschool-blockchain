/* -> blockchain.h -> */
/* llist.h stdint.h hblk_crypto.h */
#include "transaction.h"
/* fprintf */
#include <stdio.h>
/* memcmp */
#include <string.h>


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
 * tx_in_sign - signs a transaction input, based on the origin transaction id
 *
 * @in: points to the transaction input structure to sign
 * @tx_id: contains the ID (hash) of the transaction the transaction input to
 *   sign is stored in
 * @sender: contains the private key of the receiver of the coins contained in
 *   the transaction output referenced by the transaction input
 * @all_unspent: list of all unspent transaction outputs to date
 *
 * Return: pointer to the resulting signature structure upon success, or NULL
 *   upon failure
 */
sig_t *tx_in_sign(tx_in_t *in, uint8_t const tx_id[SHA256_DIGEST_LENGTH],
		  EC_KEY const *sender, llist_t *all_unspent)
{
	uint8_t sender_pub[EC_PUB_LEN];
	unspent_tx_out_t *unspent_tx_out;

	if (!in || !tx_id || !sender || !all_unspent)
	{
		fprintf(stderr, "tx_in_sign: NULL parameter(s)\n");
		return (NULL);
	}

	if (ec_to_pub(sender, sender_pub) == NULL)
	{
		fprintf(stderr, "tx_in_sign: ec_to_pub failure\n");
		return (NULL);
	}

	unspent_tx_out = llist_find_node(all_unspent,
					 (node_ident_t)matchUnspentOut,
					 in);
	if (!unspent_tx_out)
	{
		fprintf(stderr, "tx_in_sign: llist_find_node failure\n");
		return (NULL);
	}

	if (memcmp(unspent_tx_out->out.pub, sender_pub, EC_PUB_LEN) != 0)
	{
		fprintf(stderr, "tx_in_sign: input origin public key %s\n",
			"does not match public key dervied from `sender`");
		return (NULL);
	}

	if (ec_sign(sender, tx_id, SHA256_DIGEST_LENGTH, &(in->sig)) == NULL)
	{
		fprintf(stderr, "tx_in_sign: ec_sign failure\n");
		return (NULL);
	}

	return (&(in->sig));
}


/*
 * strE_LLIST needs blockchain_create.c, which is not used in automatic grading
 *		fprintf(stderr, "tx_in_sign: llist_find_node: %s\n",
 *                      strE_LLIST(llist_errno));
 */
