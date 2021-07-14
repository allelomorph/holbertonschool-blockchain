#ifndef HBLK_CRYPTO_H
#define HBLK_CRYPTO_H

/* int8_t uint8_t */
#include <stdint.h>
/* size_t */
#include <stddef.h>
/* SHA256_DIGEST_LENGTH */
#include <openssl/sha.h>
/* EC_KEY */
#include <openssl/ec.h>

/* EC_PUB_LEN not found in /usr/include/openssl/ */


/* sha256.c */
uint8_t *sha256(int8_t const *s, size_t len,
		uint8_t digest[SHA256_DIGEST_LENGTH]);

/* ec_create.c */
EC_KEY *ec_create(void);

/* ec_to_pub.c */
uint8_t *ec_to_pub(EC_KEY const *key, uint8_t pub[EC_PUB_LEN]);

/* ec_from_pub.c */
EC_KEY *ec_from_pub(uint8_t const pub[EC_PUB_LEN]);

/* ec_save.c */
int ec_save(EC_KEY *key, char const *folder);

/* ec_load.c */
EC_KEY *ec_load(char const *folder);

/* ec_sign.c */
uint8_t *ec_sign(EC_KEY const *key, uint8_t const *msg, size_t msglen,
		 sig_t *sig);

/* ec_verify.c */
int ec_verify(EC_KEY const *key, uint8_t const *msg, size_t msglen,
	      sig_t const *sig);


#endif /* HBLK_CRYPTO_H */
