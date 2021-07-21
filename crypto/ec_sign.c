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
/* memset */
#include <string.h>


/**
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
 * Note: See OpenSSLGlobalCleanup.c for how to address memory leak of `still
 *   reachable: 416 bytes in 6 blocks` reported by valgrind after ECDSA_sign
 *   subroutine calls.
 *
 * Return: pointer to the signature buffer upon success (sig->sig), or NULL
 *   upon failure
 */
uint8_t *ec_sign(EC_KEY const *key, uint8_t const *msg, size_t msglen,
		 sig_t *sig)
{
	unsigned int sig_len;
	const EC_GROUP *ec_group;

	if (!key || !msg || !sig)
	{
		fprintf(stderr, "ec_sign: NULL parameter(s)\n");
		return (NULL);
	}

	if (!EC_KEY_check_key(key))
	{
		fprintf(stderr, "ec_sign: key verification failed\n");
		return (NULL);
	}

	ec_group = EC_KEY_get0_group(key);
	if (!ec_group || EC_GROUP_get_curve_name(ec_group) != EC_CURVE ||
	    ECDSA_size(key) != SIG_MAX_LEN)
	{
		fprintf(stderr,
			"ec_sign: expecting key with secp256k1 curve\n");
		return (NULL);
	}

	memset(sig->sig, 0, SIG_MAX_LEN);
	/*
	 * `https://www.openssl.org/docs/man1.1.1/man3/ECDSA_sign.html`:
	 * "The parameter type is currently ignored." (parameter 1, `int type`)
	 */
	if (ECDSA_sign(0, msg, msglen, sig->sig,
		       &sig_len, (EC_KEY *)key) == 0)
	{
		sig->len = 0;
		fprintf(stderr, "ec_sign: ECDSA_sign failure\n");
		return (NULL);
	}

	sig->len = sig_len;
	return (sig->sig);
}
