/* assumes openssl and libssl-dev versions "1.0.1f-1ubuntu2.27 amd64" */
/* OPENSSL_VERSION_NUMBER: 0x01000106F */
/* SSLEAY_VERSION: 'OpenSSL 1.0.1f 6 Jan 2014' */


/* sig_t EC_CURVE SIG_MAX_LEN */
#include "hblk_crypto.h"
/* fprintf */
#include <stdio.h>
/* EC_KEY E_GROUP EC_KEY_check_key EC_KEY_get0_group EC_GROUP_get_curve_name */
#include <openssl/ec.h>
/* ECDSA_size ECDSA_sign */
#include <openssl/ecdsa.h>


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
 * Note: See OpenSSLGlobalCleanup for how to address memory leak of `still
 *   reachable: 416 bytes in 6 blocks` reported by valgrind after ECDSA_sign
 *   subroutine calls.
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

	if (sig->len > SIG_MAX_LEN || sig->len == 0)
	{
		fprintf(stderr, sig->len ?
			"ec_verify: signature over max length\n" :
			"ec_verify: signature length of 0\n");
		return (0);
	}

	if (ECDSA_verify(0, msg, (int)msglen, sig->sig,
			 (int)sig->len, (EC_KEY *)key) == 0)
	{
		fprintf(stderr, "ec_verify: ECDSA_verify failure\n");
		return (0);
	}

	return (1);
}
