/* assumes openssl and libssl-dev versions "1.0.1f-1ubuntu2.27 amd64" */
/* OPENSSL_VERSION_NUMBER: 0x01000106F */
/* SSLEAY_VERSION: 'OpenSSL 1.0.1f 6 Jan 2014' */


/* EC_CURVE EC_PUB_LEN */
#include "hblk_crypto.h"
/* EC_POINT EC_POINT_new EC_POINT_hex2point EC_KEY EC_GROUP EC_POINT_free */
/* EC_KEY_new_by_curve_name EC_KEY_get0_group EC_KEY_set_public_key */
#include <openssl/ec.h>
/* BN_CTX  BN_CTX_new BN_CTX_free */
#include <openssl/bn.h>
/* OPENSSL_malloc OPENSSL_free */
#include <openssl/crypto.h>
/* fprintf sprintf */
#include <stdio.h>
/* memset */
#include <string.h>


/**
 * byteArrayToEC_POINT - converts byte array representation of a public key to
 *   an EC_POINT opaque structure
 *
 * @ec_group: pointer to the EC_GROUP structure representing the type of
 *   elliptical curve to place EC_POINT on
 * @bn_ctx: pointer to the BN_CTX (BINGUM context) structure that will be used
 *   to more efficiently obtain temporary BIGNUMs during the calculations
 * @pub: byte array containing the uncompressed representation of the public
 *   key/point on the curve
 *
 * Return: pointer to new EC_POINT, or NULL upon failure
 */
EC_POINT *byteArrayToEC_POINT(const EC_GROUP *ec_group, BN_CTX *bn_ctx,
			      const uint8_t pub[EC_PUB_LEN])
{
	EC_POINT *ec_point;
	char *pub_hex_str;
	int i, j, buf_len;

	if (!ec_group || !bn_ctx || !pub)
	{
		fprintf(stderr, "byteArrayToEC_POINT: NULL parameter(s)\n");
		return (NULL);
	}

	ec_point = EC_POINT_new(ec_group);
	if (!ec_point)
	{
		fprintf(stderr, "byteArrayToEC_POINT: EC_POINT_new failure\n");
		return (NULL);
	}

	buf_len = EC_PUB_LEN * 2;
	pub_hex_str = OPENSSL_malloc(sizeof(char) * buf_len);
	if (!pub_hex_str)
	{
		fprintf(stderr,
			"byteArrayToEC_POINT: OPENSSL_malloc failure\n");
		return (NULL);
	}

	memset(pub_hex_str, 0, buf_len);
	for (i = 0, j = 0; i < EC_PUB_LEN; i++, j += 2)
		sprintf(pub_hex_str + j, "%02X", (unsigned int)(pub[i]));

	ec_point = EC_POINT_hex2point(ec_group, pub_hex_str, ec_point, bn_ctx);
	if (!ec_point)
	{
		fprintf(stderr,
			"byteArrayToEC_POINT: EC_KEY_get0_group failure\n");
		return (NULL);
	}
	/* equivalent to free(3), but used for library compliance */
	OPENSSL_free(pub_hex_str);
	return (ec_point);
}


/**
 * ec_from_pub - creates an EC_KEY structure given a public key (private key
 *   will not be initialized/set)
 *
 * @pub: contains the public key to be converted
 *
 * Return: pointer to the created EC_KEY structure upon success, or NULL upon
 *   failure
 */
EC_KEY *ec_from_pub(uint8_t const pub[EC_PUB_LEN])
{
	EC_KEY *ec_key;
	const EC_GROUP *ec_group;
	BN_CTX *bn_ctx;
	EC_POINT *ec_point;

	if (!pub)
	{
		fprintf(stderr, "ec_from_pub: NULL parameter\n");
		return (NULL);
	}
	ec_key = EC_KEY_new_by_curve_name(EC_CURVE);
	if (!ec_key)
	{
		fprintf(stderr,
			"ec_from_pub: EC_KEY_new_by_curve_name failure\n");
		return (NULL);
	}
	ec_group = EC_KEY_get0_group(ec_key);
	if (!ec_group)
	{
		fprintf(stderr, "ec_from_pub: EC_KEY_get0_group failure\n");
		return (NULL);
	}
	/* ctx used to more efficiently obtain temporary BIGNUMs */
	bn_ctx = BN_CTX_new();
	if (!bn_ctx)
	{
		fprintf(stderr, "ec_from_pub: BN_CTX_new failure\n");
		return (NULL);
	}
	ec_point = byteArrayToEC_POINT(ec_group, bn_ctx, pub);
	if (EC_KEY_set_public_key(ec_key, ec_point) == 0)
	{
		fprintf(stderr, "ec_from_pub: EC_KEY_set_public_key failure\n");
		return (NULL);
	}
	BN_CTX_free(bn_ctx);
	EC_POINT_free(ec_point);
	return (ec_key);
}
