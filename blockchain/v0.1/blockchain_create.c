#include "blockchain.h"


/**
 * blockchain_create - creates and initializes a blockchain structure
 *
 * Note: The blockchain contains one block upon creation, known as the
 *   Genesis Block. Its content is static, and pre-defined as follows:
 *      index: 0
 *      difficulty: 0
 *      timestamp: 1537578000 (similar to glibc time_t, but unsigned)
 *      nonce: 0
 *	prev_hash: (32 0x00 bytes)
 *	data: "Holberton School" (16 bytes)
 *      hash: c52c26c8b5461639635d8edf2a97d48d0c8e0009c817f2b1d3d7ff2f04515803
 *        (32 bytes)
 *
 * Return: newly allocated blockchain, or NULL on failure
 */
blockchain_t *blockchain_create(void)
{
}
