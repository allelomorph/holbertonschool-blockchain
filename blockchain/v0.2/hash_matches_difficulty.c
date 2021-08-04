#include "blockchain.h"

/**
 * hash_matches_difficulty - checks whether a given hash matches a given
 *   difficulty
 *
 * @hash: hash to check
 * @difficulty: minimum difficulty the hash should match
 *
 * Return: 1 if difficulty is appropriate, 0 if not or on failure
 */
int hash_matches_difficulty(uint8_t const hash[SHA256_DIGEST_LENGTH],
			    uint32_t difficulty)
{
	if (!hash)
	{
		fprintf(stderr, "hash_matches_difficulty: NULL parameter\n");
		return (0);
	}
}
