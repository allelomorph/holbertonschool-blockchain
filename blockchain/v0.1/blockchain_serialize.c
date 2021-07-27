/* block_t blockchain_t strE_LLIST bc_file_hdr_t HBLK_MAG HBLK_VER */
#include "blockchain.h"
/* lstat `struct stat` S_ISDIR */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
/* errno ENOENT */
#include <errno.h>
/* fprintf perror write */
#include <stdio.h>
/* open O_* S_* close */
#include <fcntl.h>
/* _get_endianness */
#include "provided/endianness.h"
/* llist_* */
#include <llist.h>


/**
 * writeFileFromPath - validates a path and opens a file descriptor in
 *   write-only mode at that path
 *
 * @path: contains the potential path to a file to open
 *
 * Return: file descriptor upon success, or -1 upon failure
 */
int writeFileFromPath(char const *path)
{
	struct stat st;
	int fd;

	if (lstat(path, &st) == -1)
	{
		if (errno != ENOENT)
		{
			perror("writeFileFromPath: lstat");
			return (-1);
		}
	}
	else if (S_ISDIR(st.st_mode))
	{
		fprintf(stderr, "writeFileFromPath: path is a directory\n");
		return (-1);
	}

	fd = open(path, O_CREAT | O_TRUNC | O_WRONLY, /* 0664 */
		  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	if (fd == -1)
	{
		if (errno == ENOENT)
		{
			fprintf(stderr, "writeFileFromPath: %s\n",
				"some directory in path does not exist");
		}
		else
			perror("writeFileFromPath: open");
		return (-1);
	}

	return (fd);
}


/**
 * writeBlkchnFileHdr - writes a serialized file header to a file for storage
 *   of a blockchain
 *
 * @fd: file descriptor open for writing
 * @blockchain: pointer to the blockchain to be serialized
 *
 * Return: 0 on success, or 1 upon failure
 */
int writeBlkchnFileHdr(int fd, const blockchain_t *blockchain)
{
	bc_file_hdr_t header = { HBLK_MAG, HBLK_VER, 0, 0 };
	int block_ct;

	if (!blockchain)
	{
		fprintf(stderr, "writeBlkchnFileHdr: NULL parameter\n");
		return (1);
	}

	header.hblk_endian = _get_endianness();
	if (header.hblk_endian == 0)
	{
		fprintf(stderr,
			"writeBlkchnFileHdr: _get_endianness failure\n");
		return (1);
	}

	block_ct = llist_size(blockchain->chain);
	if (block_ct == -1)
	{
		fprintf(stderr, "writeBlkchnFileHdr: llist_size: %s\n",
			strE_LLIST(llist_errno));
		return (1);
	}
	header.hblk_blocks = (uint32_t)block_ct;

	if (write(fd, &header, sizeof(bc_file_hdr_t)) == -1)
	{
		perror("writeBlkchnFileHdr: write");
		return (1);
	}

	return (0);
}


/**
 * writeBlocks - writes serialized blocks to a file for storage of a blockchain
 *
 * @fd: file descriptor open for writing
 * @blockchain: pointer to the blockchain to be serialized
 *
 * Return: 0 on success, or 1 upon failure
 */
int writeBlocks(int fd, const blockchain_t *blockchain)
{
	int i, block_ct;
	block_t *block = NULL;

	if (!blockchain)
	{
		fprintf(stderr, "writeBlocks: NULL parameter\n");
		return (1);
	}

	block_ct = llist_size(blockchain->chain);
	if (block_ct == -1)
	{
		fprintf(stderr, "writeBlocks: llist_size: %s\n",
			strE_LLIST(llist_errno));
		return (1);
	}

	/* assumes header has already been written to fd */
	for (i = 0; i < block_ct; i++)
	{
		block = (block_t *)llist_get_node_at(blockchain->chain, i);
		if (!block)
		{
			fprintf(stderr, "writeBlocks: llist_get_node_at: %s\n",
				strE_LLIST(llist_errno));
			return (1);
		}

		if (write(fd, &(block->info), sizeof(block_info_t)) == -1 ||
		    write(fd, &(block->data.len), sizeof(uint32_t)) == -1 ||
		    write(fd, &(block->data.buffer), block->data.len) == -1 ||
		    write(fd, &(block->hash), SHA256_DIGEST_LENGTH) == -1)
		{
			perror("writeBlocks: write");
			return (1);
		}
	}

	return (0);
}


/**
 * blockchain_serialize - serializes a blockchain into a file
 *
 * @blockchain: pointer to the blockchain to be serialized
 * @path: contains the path to a file into which to serialize the blockchain
 *
 * Note: blockchain will be serialized in the following format:
 *
 *  File Header:
 *  Offset Size(b) Field        Purpose
 *  0x00   4       hblk_magic   "HBLK" (48 42 4c 4b); identifies the file as a
 *                                valid serialized blockchain format
 *  0x04   3       hblk_version Identifies version of blockchain serialization;
 *                                format is X.Y, X and Y are ASCII numerals
 *  0x07   1       hblk_endian  Set to 1 or 2 to signify little or big
 *                                endianness, respectively; affects
 *                                interpretation of multi-byte fields
 *  0x08   4       hblk_blocks  Number of blocks in the blockchain; endianness
 *                                dependent
 *  0x0C   ?       blocks       List of blocks
 *
 *  Block: serialized contiguously, the first starting at offset 0x0C
 *  Offset Size(b)  Field      Purpose
 *  0x00   4        index      Block index in the blockchain; endianness
 *                               dependent
 *  0x04   4        difficulty Block difficulty; endianness dependent
 *  0x08   8        timestamp  Time of block creation (UNIX timestamp);
 *                               endianness dependent
 *  0x10   8        nonce      Salt value used to alter the block hash;
 *                               endianness dependent
 *  0x18   32       prev_hash  Hash of the previous block in the blockchain
 *  0x38   4        data_len   Block data size (in bytes); endianness dependent
 *  0x3C   data_len data       Block data
 *  0x3C   32       hash       Block hash
 *  + data_len
 *
 * Return: 0 upon success, or -1 upon failure
 */
int blockchain_serialize(blockchain_t const *blockchain, char const *path)
{
	int fd;

	if (!blockchain || !path)
	{
		fprintf(stderr, "blockchain_serialize: NULL parameter(s)\n");
		return (-1);
	}

	fd = writeFileFromPath(path);
	if (fd == -1)
		return (-1);

	if (writeBlkchnFileHdr(fd, blockchain) != 0)
		return (-1);

	if (writeBlocks(fd, blockchain) != 0)
		return (-1);

	close(fd);
	return (0);
}
