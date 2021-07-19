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

/**
 * FILE_FromDir - given a folder and filename, produces an open FILE pointer
 *   to a new file, or for overwriting the existing one in the same file path
 *
 * @folder: path to folder containing file to create/overwrite
 * @filename: name of file to create/overwrite
 *
 * Return: FILE pointer to file on success, NULL on failure
 */
FILE *FILE_FromDir(char const *folder, const char *filename)
{
	struct stat st;
	char file_path[PATH_MAX];
	FILE *dest_file;

	if (!folder || !filename)
	{
		fprintf(stderr, "FILE_FromDir: NULL parameter(s)\n");
		return (NULL);
	}

	if (lstat(folder, &st) == -1)
	{
		perror("FILE_FromDir: lstat");
		return (NULL);
	}

	if (!S_ISDIR(st.st_mode))
	{
		fprintf(stderr,
			"FILE_FromDir: `folder` is not a directory\n");
		return (NULL);
	}

	sprintf(file_path, "%s/%s", folder, filename);
	dest_file = fopen(file_path, "w");
	if (!dest_file)
	{
		perror("FILE_FromDir: fopen");
		return (NULL);
	}

	return (dest_file);
}


/*
 * Is a callback function needed for sending password protected keys to
 * PEM_write_ECPrivateKey? See "PEM FUNCTION ARGUMENTS" in
 * `https://www.openssl.org/docs/man1.1.1/man3/pem_password_cb.html`.
 * Relevant function prototypes constructed from openssl/pem.h macros:
 *
 * int PEM_write_bio_ECPrivateKey(BIO *bp, EC_KEY *x, const EVP_CIPHER *enc,
 *			       unsigned char *kstr, int klen,
 *			       pem_password_cb *cb, void *u);
 * int PEM_write_ECPrivateKey(FILE *fp, EC_KEY *x, const EVP_CIPHER *enc,
 *			   unsigned char *kstr, int klen,
 *			   pem_password_cb *cb, void *u);
 */
/**
 * ec_save - saves an existing EC key pair on the disk
 *
 * @key: points to the EC key pair to be saved on disk
 * @folder:  path to the folder in which to save the keys
 *
 * Note: PEM_write_* calls to BIO_new -> BIO_set -> ... -> CRYPTO_malloc ->
 *   malloc will allocate data not freed by BIO_free or BIO_free_all. At
 *   https://www.openssl.org/docs/faq.html#PROG14 "I think I've detected a
 *   memory leak..." this is described as "an OpenSSL internal table that is
 *   allocated when an application starts up". Chosen for its relationship to
 *   CRYPTO_malloc, CRYPTO_cleanup_all_ex_data is mentioned as an
 *   application-global (not thread safe) option, however it is not in the man
 *   pages.
 *
 * Return: 1 on success, 0 on failure
 */
int ec_save(EC_KEY *key, char const *folder)
{
	FILE *pub_key_file, *pri_key_file;

	if (!key || !folder)
	{
		fprintf(stderr, "ec_save: NULL parameter(s)\n");
		return (0);
	}
	if (!EC_KEY_check_key(key))
	{
		fprintf(stderr, "ec_save: key verification failed\n");
		return (0);
	}
	if (mkdir(folder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1 &&
	    errno != EEXIST)
	{
		perror("ec_save: mkdir");
		return (0);
	}
	pri_key_file = FILE_FromDir(folder, PRI_FILENAME);
	pub_key_file = FILE_FromDir(folder, PUB_FILENAME);
	if (!pub_key_file || !pri_key_file)
		return (0);
	if (PEM_write_ECPrivateKey(pri_key_file, key,
				   NULL, NULL, 0, NULL, NULL) == 0)
	{
		fprintf(stderr, "ec_save: PEM_write_ECPrivateKey failure\n");
		return (0);
	}
	if (PEM_write_EC_PUBKEY(pub_key_file, key) == 0)
	{
		fprintf(stderr, "ec_save: PEM_write_EC_PUBKEY failure\n");
		return (0);
	}
	if (fclose(pri_key_file) != 0 || fclose(pub_key_file) != 0)
	{
		perror("ec_save: fclose");
		return (0);
	}
	CRYPTO_cleanup_all_ex_data();
	return (1);
}
