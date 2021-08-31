/* mpl_file_hdr_t HMPL_MAG MEMPOOL_PATH_DFLT */
#include "hblk_cli.h"
/* printf fprintf perror write remove */
#include <stdio.h>
/* lstat `struct stat` */
#include <sys/types.h>
#include <sys/stat.h>
/* lstat */
#include <unistd.h>
/* strlen strerror */
#include <string.h>
/* PATH_MAX */
#include <linux/limits.h>
/* _get_endianness */
#include "provided/endianness.h"


/**
 * writeMplFileHdr - writes a serialized file header to a file for storage
 *   of a blockchain mempool
 *
 * @fd: file descriptor open for writing
 * @mempool: pointer to the blockchain mempool to be serialized
 *
 * File Header format:
 *  Offset Size(b) Field        Purpose
 *  0x00   4       hmpl_magic   "HMPL" (48 4d 50 4c); identifies the file as a
 *                                valid serialized blockchain mempool format
 *  0x04   3       hblk_version Identifies version of blockchain serialization;
 *                                format is X.Y, X and Y are ASCII numerals
 *  0x07   1       hmpl_endian  Set to 1 or 2 to signify little or big
 *                                endianness, respectively; affects
 *                                interpretation of multi-byte fields
 *  0x08   4       hmpl_txs     Number of transactions in the blockchain
 *                                mempool; endianness dependent
 *  0x0C   ?       txs          List of transactions, see writeTransaction
 *
 * Return: 0 on success, or 1 upon failure
 */
int writeMplFileHdr(int fd, const llist_t *mempool)
{
	mpl_file_hdr_t header = { HMPL_MAG, HBLK_VER, 0, 0 };
	int tx_ct;

	if (!mempool)
	{
		fprintf(stderr, "writeMplFileHdr: NULL parameter\n");
		return (1);
	}

	header.hmpl_endian = _get_endianness();
	if (header.hmpl_endian == 0)
	{
		fprintf(stderr,
			"writeMplFileHdr: _get_endianness failure\n");
		return (1);
	}

	tx_ct = llist_size((llist_t *)mempool);
	if (tx_ct == -1)
	{
		fprintf(stderr, "writeMplFileHdr: llist_size: %s\n",
			strE_LLIST(llist_errno));
		return (1);
	}
	header.hmpl_txs = (uint32_t)tx_ct;

	if (write(fd, &header, sizeof(mpl_file_hdr_t)) == -1)
	{
		perror("writeMplFileHdr: write");
		return (1);
	}

	return (0);
}


/**
 * mempool_serialize - serializes a blockchain mempool into a file
 *
 * @mempool: pointer to the blockchain mempool to be serialized
 * @path: contains the path to a file into which to serialize the blockchain
 *   mempool
 *
 * Note: blockchain mempool will be serialized in the following format:
 *  File header (see writeMplFileHdr)
 *  Transactions (see writeTransaction,) with each transaction
 *    containing:
 *      Number of inputs and outputs
 *      Inputs (see writeInput)
 *      Outputs (see writeOutput)
 *
 * Return: 0 upon success, or -1 upon failure
 */
int mempool_serialize(const llist_t *mempool, const char *path)
{
	int fd;

	if (!mempool || !path)
	{
		fprintf(stderr, "mempool_serialize: NULL parameter(s)\n");
		return (-1);
	}

	fd = pathToWriteFD(path);
	if (fd == -1)
		return (-1);

	if (writeMplFileHdr(fd, mempool) != 0)
	{
		close(fd);
		remove(path);
		return (-1);
	}

	if (llist_for_each((llist_t *)mempool,
			   (node_func_t)writeTransaction, &fd) < 0)
	{
		fprintf(stderr,
			"blockchain_serialize: llist_for_each failure\n");
		close(fd);
		remove(path);
		return (-1);
	}

	close(fd);
	return (0);
}


/**
 * cmd_mempool_save - saves a blockchain mempool from the current CLI session
 *   to a given path
 *
 * @path: user provided path to the file from which to load the blockchain
 *   mempool, or NULL for the default
 * @arg2: dummy arg to conform to cmd_fp_t typedef
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 *
 * Return: 0 on success, 1 on failure
 */
int cmd_mempool_save(char *path, char *arg2, cli_state_t *cli_state)
{
	struct stat st;
	char dir_path[PATH_MAX];
	int64_t i;

	(void)arg2;
	if (!cli_state)
	{
		fprintf(stderr, "cmd_mempool_save: NULL cli_state parameter\n");
		return (1);
	}

	if (!path || !path[0])
	{
		printf(TAB4 "No mempool file path provided, using default\n");
		path = MEMPOOL_PATH_DFLT;
	}

	/* ENOENT could be the file itself, or a directory in its path */
	/* So we test the parent directory first */
	strcpy(dir_path, path);
	for (i = (int64_t)strlen(dir_path) - 1;
	     i >= 0 && dir_path[i] != '/'; i--)
		dir_path[i] = '\0';

	if (*dir_path && lstat(dir_path, &st) == -1)
	{
		printf(TAB4 "Failed to save mempool in parent '%s': %s\n",
		       dir_path, strerror(errno));
		return (1);
	}

	if (lstat(path, &st) != -1 && !S_ISREG(st.st_mode))
	{
		printf(TAB4 "Failed to save mempool to '%s': %s\n",
		       path, "Not a regular file, overwriting is not advised");
		return (1);
	}

	if (!cli_state->mempool)
	{
		printf(TAB4 "Failed to save mempool to '%s': %s\n",
		       path, "Session mempool is missing (NULL)");
		return (1);
	}

	if (mempool_serialize(cli_state->mempool, path) != 0)
	{
		printf(TAB4 "Failed to save mempool to '%s'\n", path);
		return (1);
	}

	printf(TAB4 "Saved mempool to '%s'\n", path);
	return (0);
}
