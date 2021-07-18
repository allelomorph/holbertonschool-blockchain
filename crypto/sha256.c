/* includes stdint.h and stddef.h */
#include "hblk_crypto.h"
/* SHA256_DIGEST_LENGTH SHA256_CTX SHA256_Init SHA256_Update SHA256_Final */
#include <openssl/sha.h>
/* fprintf */
#include <stdio.h>


/* assumes openssl and libssl-dev versions "1.0.1f-1ubuntu2.27 amd64" */
/* OPENSSL_VERSION_NUMBER: 0x01000106F */
/* SSLEAY_VERSION: 'OpenSSL 1.0.1f 6 Jan 2014' */

/*
 * The three SHA256_* functions were chosen to make the process more explicit
 * for learning purposes and to track errors, but the same could be achieved
 * with a single call to:
 *      return ((uint8_t *)SHA256((const unsigned char *)s,
 *				  (unsigned long)len,
 *				  (unsigned char *)digest));
 */

/**
 * sha256 - computes the hash of a sequence of bytes
 *
 * @s: sequence of bytes to be hashed
 * @len: number of bytes to hash in `s`
 * @digest: resulting hash stored in this buffer, if not NULL
 *
 * Return: pointer to `digest`, or NULL on failure
 */
uint8_t *sha256(int8_t const *s, size_t len,
		uint8_t digest[SHA256_DIGEST_LENGTH])
{
	SHA256_CTX ctx;

	if (!digest)
	{
		fprintf(stderr, "sha256: NULL digest\n");
		return (NULL);
	}

	if (SHA256_Init(&ctx) == 0)
	{
		fprintf(stderr, "sha256: SHA256_Init failure\n");
		return (NULL);
	}

	if (SHA256_Update(&ctx, (const void *)s, (unsigned long)len) == 0)
	{
		fprintf(stderr, "sha256: SHA256_Update failure\n");
		return (NULL);
	}

	/* *_Final also erases context, per sha(3) */
	if (SHA256_Final((unsigned char *)digest, &ctx) == 0)
	{
		fprintf(stderr, "sha256: SHA256_Final failure\n");
		return (NULL);
	}

	return (digest);
}
