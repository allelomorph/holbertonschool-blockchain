/* adapted from blockchain/v0.3/provided/_blockchain_print.c */

#include "blockchain.h"
/* TAB4 */
#include "hblk_cli.h"
/* printf */
#include <stdio.h>
/* memcpy strlen */
#include <string.h>


/**
 * _block_print - used as `action` for llist_for_each to
 *   visit each block in blockchain list, and print its contents
 *
 * @block: pointer to block in list, as iterated through by
 *   llist_for_each
 * @index: index of `block` in list, as iterated through by llist_for_each
 * @indent: indentation string to use in printing
 *
 * Return: 0 on incremental success (llist_for_each can continue,)
 *   and -2 on failure (-1 reserved for llist_for_each errors)
 */
static int _block_print(block_t const *block, unsigned int index,
			char const *indent)
{
	char indent2[20] = {0};

	(void)index;
	if (!block)
	{
		printf("%snil\n", indent);
		return (-2);
	}

	printf("%sBlock: {\n", indent);

	printf("%s" TAB4 "info: {\n", indent);
	printf("%s" TAB4 TAB4 "index: %u,\n", indent, block->info.index);
	printf("%s" TAB4 TAB4 "difficulty: %u,\n", indent, block->info.difficulty);
	printf("%s" TAB4 TAB4 "timestamp: %lu,\n", indent, block->info.timestamp);
	printf("%s" TAB4 TAB4 "nonce: %lu,\n", indent, block->info.nonce);
	printf("%s" TAB4 TAB4 "prev_hash: ", indent);
	_print_hex_buffer(block->info.prev_hash, SHA256_DIGEST_LENGTH);
	printf("\n%s" TAB4 "},\n", indent);

	printf("%s" TAB4 "data: {\n", indent);
	printf("%s" TAB4 TAB4 "buffer: \"%s\",\n", indent, block->data.buffer);
	printf("%s" TAB4 TAB4 "len: %u\n", indent, block->data.len);
	printf("%s" TAB4 "},\n", indent);

	printf("%s" TAB4 "transactions [%d]: [\n", indent,
		llist_size(block->transactions));
	memcpy(indent2, indent, strlen(indent));
	memcpy(indent2 + strlen(indent), TAB4 TAB4, strlen(TAB4 TAB4));
	llist_for_each(block->transactions,
		(node_func_t)_transaction_print_loop, (void *)indent2);
	printf("%s" TAB4 "]\n", indent);

	printf("%s" TAB4 "hash: ", indent);
	_print_hex_buffer(block->hash, SHA256_DIGEST_LENGTH);

	printf("\n%s}\n", indent);

	return (0);
}


/**
 * _block_print_brief - used as `action` for llist_for_each to
 *   visit each block in blockchain list, and print its contents
 *
 * @block: pointer to block in list, as iterated through by
 *   llist_for_each
 * @index: index of `block` in list, as iterated through by llist_for_each
 * @indent: indentation string to use in printing
 *
 * Return: 0 on incremental success (llist_for_each can continue,)
 *   and -2 on failure (-1 reserved for llist_for_each errors)
 */
static int _block_print_brief(block_t const *block, unsigned int index,
			      char const *indent)
{
	char indent2[20] = {0};

	(void)index;
	if (!block)
	{
		printf("%snil\n", indent);
		return (0);
	}

	printf("%sBlock: {\n", indent);

	printf("%s" TAB4 "info: { ", indent);
	printf("%u, ", block->info.index);
	printf("%u, ", block->info.difficulty);
	printf("%lu, ", block->info.timestamp);
	printf("%lu, ", block->info.nonce);
	_print_hex_buffer(block->info.prev_hash, SHA256_DIGEST_LENGTH);
	printf(" },\n");

	printf("%s\tdata: { ", indent);
	printf("\"%s\", ", block->data.buffer);
	printf("%u", block->data.len);
	printf(" },\n");

	printf("%s" TAB4 "transactions [%d]: [\n", indent,
		llist_size(block->transactions));
	memcpy(indent2, indent, strlen(indent));
	memcpy(indent2 + strlen(indent), TAB4 TAB4, strlen(TAB4 TAB4));
	llist_for_each(block->transactions,
		(node_func_t)_transaction_print_brief_loop, (void *)indent2);
	printf("%s" TAB4 "]\n", indent);

	printf("%s" TAB4 "hash: ", indent);
	_print_hex_buffer(block->hash, SHA256_DIGEST_LENGTH);

	printf("\n%s}\n", indent);

	return (0);
}


/**
 * _blockchain_print - Prints an entire blockchain
 *
 * @blockchain: Pointer to the blockchain to be printed
 */
void _blockchain_print(blockchain_t const *blockchain)
{
	printf("Blockchain: {\n");

	printf(TAB4 "chain [%d]: [\n", llist_size(blockchain->chain));
	llist_for_each(blockchain->chain,
		(node_func_t)_block_print, TAB4 TAB4);
	printf(TAB4 "]\n");

	printf("}\n");
	fflush(NULL);
}


/**
 * _blockchain_print_brief - Prints an entire blockchain (brief mode)
 *
 * @blockchain: Pointer to the blockchain to be printed
 */
void _blockchain_print_brief(blockchain_t const *blockchain)
{
	printf("Blockchain: {\n");

	printf(TAB4 "chain [%d]: [\n", llist_size(blockchain->chain));
	llist_for_each(blockchain->chain,
		(node_func_t)_block_print_brief, TAB4 TAB4);
	printf(TAB4 "]\n");

	printf("}\n");
	fflush(NULL);
}
