/* assumes openssl and libssl-dev versions "1.0.1f-1ubuntu2.27 amd64" */
/* OPENSSL_VERSION_NUMBER: 0x01000106F */
/* SSLEAY_VERSION: 'OpenSSL 1.0.1f 6 Jan 2014' */


/* EC_PUB_LEN */
#include "hblk_crypto.h"
/* EC_POINT EC_GROUP EC_KEY_get0_public_key EC_KEY_get0_group */
/* EC_POINT_point2hex POINT_CONVERSION_UNCOMPRESSED */
#include <openssl/ec.h>
/* BN_CTX BN_CTX_new BN_CTX_free */
#include <openssl/bn.h>
/* OPENSSL_free */
#include <openssl/crypto.h>
/* fprintf sscanf perror */
#include <stdio.h>
/* errno */
#include <errno.h>
/* memset */
#include <string.h>


/**
 * byteArrayFromEC_POINT - derives representation of EC_POINT public key as an
 *   array of bytes
 *
 * @ec_point: pointer to the EC_POINT structure to reformat as uint8_t array
 * @ec_group: pointer to the EC_GROUP structure derived from the same EC_KEY
 *   as `ec_point`
 * @bn_ctx: pointer to the BN_CTX (BINGUM context) structure that will be used
 *   to more efficiently obtain temporary BIGNUMs during the calculations
 * @pub: address at which to store the extracted public key (The key shall not
 *   be compressed)
 *
 * Return: pointer to `pub`, or NULL upon failure
 */
uint8_t *byteArrayFromEC_POINT(const EC_POINT *ec_point,
			       const EC_GROUP *ec_group,
			       BN_CTX *bn_ctx, uint8_t pub[EC_PUB_LEN])
{
	char *pub_temp;
	int i, j;

	if (!ec_point || !ec_group || !bn_ctx || !pub)
	{
		fprintf(stderr, "byteArrayFromEC_POINT: NULL parameter(s)\n");
		return (NULL);
	}

	pub_temp = EC_POINT_point2hex(ec_group, ec_point,
				      POINT_CONVERSION_UNCOMPRESSED, bn_ctx);
	if (!pub_temp)
	{
		fprintf(stderr,
			"byteArrayFromEC_POINT: EC_POINT_point2hex failure\n");
		return (NULL);
	}

	memset(pub, 0, EC_PUB_LEN);

	for (i = 0, j = 0; i < EC_PUB_LEN; i++, j += 2)
	{
		if (sscanf(pub_temp + j, "%02X",
			   (unsigned int *)(pub + i)) != 1)
		{
			perror("byteArrayFromEC_POINT: sscanf");
			errno = 0;
			return (NULL);
		}
	}

	/* equivalent to free(3), but used for library compliance */
	OPENSSL_free(pub_temp);

	return (pub);
}


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
	const EC_POINT *ec_point;
	const EC_GROUP *ec_group;
	BN_CTX *bn_ctx;

	if (!key || !pub)
	{
		fprintf(stderr, "ec_to_pub: NULL parameter(s)\n");
		return (NULL);
	}

	ec_point = EC_KEY_get0_public_key(key);
	if (!ec_point)
	{
		fprintf(stderr, "ec_to_pub: EC_KEY_get0_public_key failure\n");
		return (NULL);
	}

	ec_group = EC_KEY_get0_group(key);
	if (!ec_group)
	{
		fprintf(stderr, "ec_to_pub: EC_KEY_get0_group failure\n");
		return (NULL);
	}

	/* ctx used to more efficiently obtain temporary BIGNUMs */
	bn_ctx = BN_CTX_new();
	if (!bn_ctx)
	{
		fprintf(stderr, "ec_to_pub: BN_CTX_new failure\n");
		return (NULL);
	}

	pub = byteArrayFromEC_POINT(ec_point, ec_group, bn_ctx, pub);

	BN_CTX_free(bn_ctx);
	return (pub);
}
