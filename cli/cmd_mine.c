/* MEMPOOL_PATH_DFLT */
#include "hblk_cli.h"
/* printf fprintf */
#include <stdio.h>
/* strerror memcmp */
#include <string.h>
/* time */
#include <time.h>


/**
 * mempoolTxToBlock - used as `action` for llist_for_each to visit each
 *   pending transaction in a mempool, and add the verified ones to a
 *   new block to be mined
 *
 * @tx: pointer to a transaction in the mempool, as iterated through by
 *   llist_for_each
 * @idx: index of `tx` in the mempool, as iterated through by llist_for_each
 * @mtb_info: pointer to struct containing the new block and the list of
 *   unspent transaction outputs
 *
 * Return: 0 on incremental success (llist_for_each can continue,)
 *   or -2 on failure (-1 reserved for llist_for_each errors)
 */
static int mempoolTxToBlock(transaction_t *tx, unsigned int idx,
			    mtb_info_t *mtb_info)
{
	(void)idx;

	if (!tx || !mtb_info)
	{
		fprintf(stderr, "mempoolTxToBlock: NULL parameter(s)\n");
		return (-2);
	}

	if (transaction_is_valid(tx, mtb_info->unspent))
	{
		if (llist_add_node(mtb_info->new_block->transactions,
				   tx, ADD_NODE_REAR) != 0)
		{
			fprintf(stderr, "mempoolTxToBlock: lllist_add_node: %s\n",
				strE_LLIST(llist_errno));
			return (-2);
		}
	}

	return (0);
}


/**
 * cmd_mine - mines a new block in the blockchain: includes all valid
 *   transactions currently in the mempool plus a coinbase transaction, sets
 *   block difficulty, finds a matching hash and adds the block to the chain
 *
 * @arg1: dummy arg to conform to cmd_fp_t typedef
 * @arg2: dummy arg to conform to cmd_fp_t typedef
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 *
 * Return: 0 on success, 1 on failure
 */
int cmd_mine(char *arg1, char *arg2, cli_state_t *cli_state)
{
	block_t *prev_block, *new_block;
	transaction_t *coinbase;
	mtb_info_t mtb_info;

	(void)arg1;
	(void)arg2;
	if (!cli_state)
	{
		fprintf(stderr, "cmd_mine: NULL cli_state parameter\n");
		return (1);
	}

	prev_block = (block_t *)llist_get_tail(cli_state->blockchain->chain);
	if (!prev_block)
	{
		fprintf(stderr, "cmd_mine: llist_get_tail: %s\n",
			strE_LLIST(llist_errno));
		return (1);
	}
	new_block = block_create(prev_block, (int8_t *)"", 0);
	if (!new_block)
	{
		fprintf(stderr, "cmd_mine: block_create failure\n");
		return (1);
	}

	/* add valid mempool txs to block */
	mtb_info.new_block = new_block;
	mtb_info.unspent = cli_state->blockchain->unspent;
	if (llist_for_each(cli_state->mempool,
			   (node_func_t)mempoolTxToBlock, &mtb_info) != 0)
	{
		block_destroy(new_block);
		fprintf(stderr, "cmd_mine: llist_for_each failure\n");
		return (1);
	}
	/* delete all remaining unvalidated mempool transactions */
	llist_destroy(cli_state->mempool, 1, (node_dtor_t)transaction_destroy);
	cli_state->mempool = llist_create(MT_SUPPORT_FALSE);

	new_block->info.difficulty =
		blockchain_difficulty(cli_state->blockchain);
	new_block->info.timestamp = (uint64_t)time(NULL);

	coinbase = coinbase_create(cli_state->wallet, new_block->info.index);
	if (!coinbase)
	{
		fprintf(stderr, "cmd_mine: coinbase_create failure\n");
		return (1);
	}
	llist_add_node(new_block->transactions, coinbase, ADD_NODE_FRONT);

	block_mine(new_block);
	if (block_is_valid(new_block, prev_block,
			   cli_state->blockchain->unspent) != 0)
	{
		printf(TAB4 "Failed to mine block at index: %u\n",
		       new_block->info.index);
		block_destroy(new_block);
		return (1);
	}

	cli_state->blockchain->unspent =
		update_unspent(new_block->transactions,
			       new_block->hash,
			       cli_state->blockchain->unspent);
	if (!cli_state->blockchain->unspent)
	{
		fprintf(stderr, "cmd_mine: update_unspent failure\n");
		block_destroy(new_block);
		return (1);
	}

	if (refreshUnspentCache(cli_state) != 0)
	{
		fprintf(stderr, "cmd_mine: refreshUnspentCache failure\n");
		block_destroy(new_block);
		return (1);
	}

	if (llist_add_node(cli_state->blockchain->chain,
			   new_block, ADD_NODE_REAR) != 0)
	{
		fprintf(stderr, "cmd_mine: llist_add_node failure\n");
		block_destroy(new_block);
		return (1);
	}

	printf(TAB4 "Block mined: [%u] ", new_block->info.difficulty);
	_print_hex_buffer(new_block->hash, SHA256_DIGEST_LENGTH);
	printf("\n");
	return (0);
}
