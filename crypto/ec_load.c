/* assumes openssl and libssl-dev versions "1.0.1f-1ubuntu2.27 amd64" */
/* OPENSSL_VERSION_NUMBER: 0x01000106F */
/* SSLEAY_VERSION: 'OpenSSL 1.0.1f 6 Jan 2014' */


/* (includes stdint.h and stddef.h) */
#include "hblk_crypto.h"
/* FILE fprintf perror sprintf fopen fclose */
#include <stdio.h>
#include <errno.h>
/* EC_KEY EC_KEY_check_key */
#include <openssl/ec.h>
/* PEM_read_ECPrivateKey PEM_read_EC_PUBKEY */
#include <openssl/pem.h>
/* `struct stat` lstat */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
/* PATH_MAX */
#include <linux/limits.h>
/* strlen */
#include <string.h>


/*
 * highly redundant with W_FILE_FromDir, fopen mode could just be a parameter,
 * but for automatic grading purposes ec_save.c and ec_load.c need to both be
 * independent translation units
 */
/**
 * R_FILE_FromDir - given a folder and filename, produces an open FILE pointer
 *   to read a file
 *
 * @folder: path to folder containing file to create/overwrite
 * @filename: name of file to create/overwrite
 *
 * Return: FILE pointer to file on success, NULL on failure
 */
FILE *R_FILE_FromDir(char const *folder, const char *filename)
{
	struct stat st;
	char file_path[PATH_MAX];
	FILE *dest_file;
	int end_slash;

	if (!folder || !filename)
	{
		fprintf(stderr, "R_FILE_FromDir: NULL parameter(s)\n");
		return (NULL);
	}
	if (lstat(folder, &st) == -1)
	{
		if (errno == ENOENT)
			fprintf(stderr, "R_FILE_FromDir: fopen: '%s': %s\n",
				folder, strerror(ENOENT));
		else
			perror("R_FILE_FromDir: fopen");
		return (NULL);
	}
	if (!S_ISDIR(st.st_mode))
	{
		fprintf(stderr,
			"R_FILE_FromDir: `folder` is not a directory\n");
		return (NULL);
	}

	end_slash = (folder[strlen(folder) - 1] == '/');
	sprintf(file_path, end_slash ? "%s%s" : "%s/%s", folder, filename);
	dest_file = fopen(file_path, "r");
	if (!dest_file)
	{
		if (errno == ENOENT)
			fprintf(stderr, "R_FILE_FromDir: fopen: '%s': %s\n",
				file_path, strerror(ENOENT));
		else
			perror("R_FILE_FromDir: fopen");
		return (NULL);
	}
	return (dest_file);
}


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
 * Note: See OpenSSLGlobalCleanup.c for how to address memory leak of `still
 *   reachable: 416 bytes in 6 blocks` reported by valgrind after PEM_read*
 *   subroutine calls.
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

	pri_key_file = R_FILE_FromDir(folder, PRI_FILENAME);
	pub_key_file = R_FILE_FromDir(folder, PUB_FILENAME);
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
