#include "hblk_crypto.h"


/**
 * ec_to_pub - extracts the public key from an EC_KEY opaque structure
 *
 * @key: pointer to the EC_KEY structure to retrieve the public key from
 * @pub: address at which to store the extracted public key (The key shall not
 *   be compressed)
 *
 * Return: pointer to `pub`, or NULL upon failure
 */
uint8_t *ec_to_pub(EC_KEY const *key, uint8_t pub[EC_PUB_LEN])
{
}
