/* (includes stdint.h and stddef.h) */
#include "hblk_crypto.h"
/* FILE fprintf perror sprintf fopen fclose */
#include <stdio.h>
#include <errno.h>
/* `struct stat` lstat */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
/* PATH_MAX */
#include <linux/limits.h>
/* EC_KEY EC_KEY_check_key */
#include <openssl/ec.h>
/* PEM_write_ECPrivateKey PEM_write_EC_PUBKEY */
#include <openssl/pem.h>
/* CRYPTO_cleanup_all_ex_data */
#include <openssl/crypto.h>


/* assumes openssl and libssl-dev versions "1.0.1f-1ubuntu2.27 amd64" */
/* OPENSSL_VERSION_NUMBER: 0x01000106F */
/* SSLEAY_VERSION: 'OpenSSL 1.0.1f 6 Jan 2014' */


/*
 * Is a callback function needed for deriving password protected keys from
 * PEM_read_ECPrivateKey? See "PEM FUNCTION ARGUMENTS" in
 * `https://www.openssl.org/docs/man1.1.1/man3/pem_password_cb.html`.
 * Relevant function prototypes constructed from openssl/pem.h macros:
 *
 * EC_KEY *PEM_read_bio_ECPrivateKey(BIO *bp, EC_KEY **x,
 *				  pem_password_cb *cb, void *u);
 * EC_KEY *PEM_read_ECPrivateKey(FILE *fp, EC_KEY **x,
 *			      pem_password_cb *cb, void *u);
 */
/**
 * ec_load - loads an EC key pair from the disk
 *
 * @folder: path to the folder from which to load the keys
 *
 * Return: pointer to the created EC key pair upon success, or NULL upon
 *   failure
 */
EC_KEY *ec_load(char const *folder)
{
	EC_KEY *ec_key;
	FILE *pub_key_file, *pri_key_file;

	if (!folder)
	{
		fprintf(stderr, "ec_load: NULL parameter(s)\n");
		return (NULL);
	}

	pri_key_file = FILE_FromDir(folder, PRI_FILENAME, "r");
	pub_key_file = FILE_FromDir(folder, PUB_FILENAME, "r");
	if (!pub_key_file || !pri_key_file)
		return (NULL);

	ec_key = PEM_read_EC_PUBKEY(pub_key_file, NULL, NULL, NULL);
	if (!ec_key)
	{
		fprintf(stderr, "ec_save: PEM_read_EC_PUBKEY failure\n");
		return (NULL);
	}

	ec_key = PEM_read_ECPrivateKey(pri_key_file, &ec_key, NULL, NULL);
	if (!ec_key)
	{
		fprintf(stderr, "ec_load: PEM_read_ECPrivateKey failure\n");
		return (NULL);
	}

	if (fclose(pri_key_file) != 0 || fclose(pub_key_file) != 0)
	{
		perror("ec_load: fclose");
		return (NULL);
	}

	return (ec_key);
}
