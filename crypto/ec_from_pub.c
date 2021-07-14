#include "hblk_crypto.h"


/**
 * ec_from_pub - creates an EC_KEY structure given a public key
 *
 * @pub: contains the public key to be converted
 *
 * Return: pointer to the created EC_KEY structure upon success, or NULL upon
 *   failure
 */
EC_KEY *ec_from_pub(uint8_t const pub[EC_PUB_LEN])
{
}
