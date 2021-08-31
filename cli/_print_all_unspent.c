#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "blockchain.h"
/* TAB4 */
#include "hblk_cli.h"

void _print_hex_buffer(uint8_t const *buf, size_t len);

/**
 * _print_unspent - Print an unspent transaction output
 *
 * @unspent: Pointer to the unspent transaction output to print
 * @idx: Unused
 * @indent:  Indentation
 *
 * Return: 0
 */
static int _print_unspent(const unspent_tx_out_t *unspent, unsigned int idx,
			  const char *indent)
{
        (void)idx;

        printf("%s{\n", indent);

        printf("%s\tblock_hash: ", indent);
        _print_hex_buffer(unspent->block_hash, SHA256_DIGEST_LENGTH);
        printf("\n");

        printf("%s\ttx_id: ", indent);
        _print_hex_buffer(unspent->tx_id, SHA256_DIGEST_LENGTH);
        printf("\n");

        printf("%s\tamount: %u\n", indent, unspent->out.amount);

        printf("%s\tpub: ", indent);
        _print_hex_buffer(unspent->out.pub, EC_PUB_LEN);
        printf("\n");

        printf("%s}\n", indent);

        return (0);
}


/**
 * _print_all_unspent - Print a list of unspent transaction outputs
 *
 * @unspent: List of unspent transaction outputs to print
 */
void _print_all_unspent(llist_t *unspent)
{
        printf("Unspent transaction outputs [%u]: [\n", llist_size(unspent));

        llist_for_each(unspent, (node_func_t)_print_unspent, TAB4);

        printf("]\n");
}
