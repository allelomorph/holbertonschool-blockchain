#ifndef HBLK_CRYPTO_H
#define HBLK_CRYPTO_H


/* assumes openssl and libssl-dev versions "1.0.1f-1ubuntu2.27 amd64" */
/* OPENSSL_VERSION_NUMBER: 0x01000106F */
/* SSLEAY_VERSION: 'OpenSSL 1.0.1f 6 Jan 2014' */

/* int8_t uint8_t */
#include <stdint.h>
/* size_t */
#include <stddef.h>
/* SHA256_DIGEST_LENGTH */
#include <openssl/sha.h>
/* EC_KEY */
#include <openssl/ec.h>
/* NID_secp256k1 */
#include <openssl/obj_mac.h>


/* EC_PUB_LEN not found in /usr/include/openssl/ */
/* project example outputs are 130 chars, or 65 bytes in 2-digit hex */
#define EC_PUB_LEN 65

/* used in project correction test mains */
#define EC_CURVE NID_secp256k1
#define PUB_FILENAME "pub_key.pem"
#define PRI_FILENAME "key.pem"

/* sig_t appears in glibc signal.h 207-210 as BSD equivalent to sighandler_t */
/* `#if defined (__USE_BSD) && defined (_SIGNAL_H)` not allowed by linter */
#ifdef __USE_BSD
#ifdef _SIGNAL_H
#error "hblk_crypto.h definition of sig_t can conflict with BSD signal.h sig_t"
#endif
#endif


/**
 * struct sig_s - used to store signature performed by EC_KEY key pair
 *
 * @sig: unsigned byte array representing signature
 * @len: amount of bytes in `sig`
 */
typedef struct sig_s
{
	uint8_t *sig;
	size_t len;
} sig_t;


/* sha256.c */
uint8_t *sha256(int8_t const *s, size_t len,
		uint8_t digest[SHA256_DIGEST_LENGTH]);

/* ec_create.c */
EC_KEY *ec_create(void);

/* ec_to_pub.c */
uint8_t *byteArrayFromEC_POINT(const EC_POINT *ec_point,
			       const EC_GROUP *ec_group,
			       BN_CTX *bn_ctx, uint8_t pub[EC_PUB_LEN]);
uint8_t *ec_to_pub(EC_KEY const *key, uint8_t pub[EC_PUB_LEN]);

/* ec_from_pub.c */
EC_POINT *byteArrayToEC_POINT(const EC_GROUP *ec_group, BN_CTX *bn_ctx,
			      const uint8_t pub[EC_PUB_LEN]);
EC_KEY *ec_from_pub(uint8_t const pub[EC_PUB_LEN]);

/* ec_save.c */
FILE *W_FILE_FromDir(char const *folder, const char *filename);
int ec_save(EC_KEY *key, char const *folder);

/* ec_load.c */
FILE *R_FILE_FromDir(char const *folder, const char *filename);
EC_KEY *ec_load(char const *folder);

/* ec_sign.c */
uint8_t *ec_sign(EC_KEY const *key, uint8_t const *msg, size_t msglen,
		 sig_t *sig);

/* ec_verify.c */
int ec_verify(EC_KEY const *key, uint8_t const *msg, size_t msglen,
	      sig_t const *sig);

#endif /* HBLK_CRYPTO_H */
