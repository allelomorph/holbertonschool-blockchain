/* assumes openssl and libssl-dev versions "1.0.1f-1ubuntu2.27 amd64" */
/* OPENSSL_VERSION_NUMBER: 0x01000106F */
/* SSLEAY_VERSION: 'OpenSSL 1.0.1f 6 Jan 2014' */

/* (includes stdint.h and stddef.h) */
#include "hblk_crypto.h"
/* fprintf */
#include <stdio.h>
/* EC_KEY EC_KEY_check_key */
#include <openssl/ec.h>
/* ECDSA_size ECDSA_sign */
#include <openssl/ecdsa.h>
/* CRYPTO_cleanup_all_ex_data */
#include <openssl/crypto.h>
/* memset */
#include <string.h>


/*
 * ec_sign - signs a given set of bytes, using a given EC_KEY private key
 *
 * @key: points to the EC_KEY structure containing the private key to be used
 *   to perform the signature
 * @msg: points to the `msglen` characters to be signed
 * @msglen: count of characters in `msg`
 * @sig: address at which to store the signature, note: `sig->sig` does not
 *   need to be zero-terminated. If it is, `sig->len` should hold the size of
 *   the signature without the trailing zero byte
 *
 * Note: In cases such as ECDSA_sign, calls to ECDSA_sign_ex -> ... ->
 *   CRYPTO_malloc -> malloc will allocate data not later freed. At
 *   `https://www.openssl.org/docs/faq.html#PROG14`, "I think I've detected a
 *   memory leak...", this is described as "an OpenSSL internal table that is
 *   allocated when an application starts up". Chosen for its presumed
 *   relationship to CRYPTO_malloc, CRYPTO_cleanup_all_ex_data is mentioned as
 *   an application-global, non-thread safe option, however it is not in the
 *   1.1.1 man pages.
 *
 *   Global cleanup like CRYPTO_cleanup_all_ex_data would best be done only
 *   once in an __attribute__ ((destructor)) function after main exits, but
 *   the way that this project is compiled for automatic grading prevents it.
 *
 * Return: pointer to the signature buffer upon success (sig->sig), or NULL
 *   upon failure
 */
/**
 * ec_verify - verifies the signature of a given set of bytes, using a given
 *   EC_KEY public key
 *
 * @key: points to the EC_KEY structure containing the public key to be used
 *   to verify the signature
 * @msg: points to the msglen characters to verify the signature of
 * @msglen: count of characters in `msg`
 * @sig: points to the signature to be checked
 *
 * Return: 1 if the signature is valid, and 0 otherwise
 */
int ec_verify(EC_KEY const *key, uint8_t const *msg, size_t msglen,
	      sig_t const *sig)
{
	const EC_GROUP *ec_group;

	if (!key || !msg || !sig)
	{
		fprintf(stderr, "ec_verify: NULL parameter(s)\n");
		return (0);
	}

	if (!EC_KEY_check_key(key))
	{
		fprintf(stderr, "ec_verify: EC_KEY_check_key failure\n");
		return (0);
	}

	ec_group = EC_KEY_get0_group(key);
	if (!ec_group || EC_GROUP_get_curve_name(ec_group) != EC_CURVE ||
	    ECDSA_size(key) != SIG_MAX_LEN)
	{
		fprintf(stderr,
			"ec_verify: expecting key with secp256k1 curve\n");
		return (0);
	}

	if (sig->len > SIG_MAX_LEN)
	{
		fprintf(stderr, "ec_verify: signature longer than expected\n");
		return (0);
	}
	/*
	 * int ECDSA_verify(int type, const unsigned char *dgst, int dgstlen,
	 *		 const unsigned char *sig, int siglen, EC_KEY *eckey);
	 */
	if (ECDSA_verify(0, msg, (int)msglen, sig->sig,
			 (int)sig->len, (EC_KEY *)key) == 0)
	{
		fprintf(stderr, "ec_sign: ECDSA_verify failure\n");
		return (0);
	}

	return (1);
}
