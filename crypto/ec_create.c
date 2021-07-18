/* includes stdint.h and stddef.h */
#include "hblk_crypto.h"
/* NID_secp256k1 */
#include <openssl/obj_mac.h>
/* EC_KEY EC_GROUP EC_KEY_new EC_GROUP_new_by_curve_name */
/* EC_KEY_set_group EC_KEY_generate_key */
#include <openssl/ec.h>
/* fprintf */
#include <stdio.h>


/* assumes openssl and libssl-dev versions "1.0.1f-1ubuntu2.27 amd64" */
/* OPENSSL_VERSION_NUMBER: 0x01000106F */
/* SSLEAY_VERSION: 'OpenSSL 1.0.1f 6 Jan 2014' */

/*
 * EC_KEY generation done incrementally here for learning purposes, but the
 * first three steps could also be achieved with:
 *      ec_key = EC_KEY_new_by_curve_name(NID_secp256k1);
 */

/**
 * ec_create - creates a new EC key pair using the secp256k1
 *   (https://en.bitcoin.it/wiki/Secp256k1) elliptic curve
 *
 * Return: pointer to an EC_KEY structure, containing both the public and
 *   private keys, or NULL upon failure
 */
EC_KEY *ec_create(void)
{
	EC_KEY *ec_key;
	EC_GROUP *ec_group;

	ec_key = EC_KEY_new();
	if (!ec_key)
	{
		fprintf(stderr, "ec_create: EC_KEY_new failure\n");
		return (NULL);
	}

	ec_group = EC_GROUP_new_by_curve_name(NID_secp256k1);
	if (!ec_group)
	{
		fprintf(stderr,
			"ec_create: EC_GROUP_new_by_curve_name failure\n");
		return (NULL);
	}

	if (EC_KEY_set_group(ec_key, ec_group) == 0)
	{
		fprintf(stderr, "ec_create: EC_KEY_set_group failure\n");
		return (NULL);
	}

	/*
	 * per https://www.openssl.org/docs/man1.1.1/man3/EC_KEY_new.html:
	 * The private key is a random integer (0 < priv_key < order, where
	 * order is the order of the EC_GROUP object). The public key is an
	 * EC_POINT on the curve calculated by multiplying the generator for
	 * the curve by the private key.
	 */
	if (EC_KEY_generate_key(ec_key) == 0)
	{
		fprintf(stderr, "ec_create: EC_KEY_generate_key failure\n");
		return (NULL);
	}

	return (ec_key);
}
