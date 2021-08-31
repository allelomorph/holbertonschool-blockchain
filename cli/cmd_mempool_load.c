/* MEMPOOL_PATH_DFLT */
#include "hblk_cli.h"
/* printf fprintf */
#include <stdio.h>
/* lstat `struct stat` */
#include <sys/types.h>
#include <sys/stat.h>
/* lstat */
#include <unistd.h>
/* strerror memcmp */
#include <string.h>
/* open O_* close */
#include <fcntl.h>
/* _get_endianness _swap_endian */
#include "provided/endianness.h"


/**
 * readMplFileHdr - reads and validates a serialized file header from a
 *   file storing a blockchain
 *
 * @fd: file descriptor already open for reading
 * @local_endianness: 1 for little endian, 2 for big endian
 * @header: stores values read from file header
 *
 * Return: 0 on success, or 1 upon failure
 */
int readMplFileHdr(int fd, uint8_t local_endianness,
		   mpl_file_hdr_t *header)
{
	if (!header)
	{
		fprintf(stderr, "readMplFileHdr: NULL parameter\n");
		return (1);
	}
	if (local_endianness != 1 && local_endianness != 2)
	{
		fprintf(stderr,
			"readMplFileHdr: invalid local_endianness\n");
		return (1);
	}
	if (read(fd, header, sizeof(mpl_file_hdr_t)) == -1)
	{
		perror("readMplFileHdr: read");
		return (1);
	}
	if (memcmp(&(header->hmpl_magic), HMPL_MAG, HMPL_MAG_LEN) != 0)
	{
		fprintf(stderr,
			"readMplFileHdr: invalid magic number\n");
		return (1);
	}
	if (memcmp(&(header->hblk_version), HBLK_VER, HBLK_VER_LEN) != 0)
	{
		fprintf(stderr, "readMplFileHdr: %s\n",
			"serialized with incompatible version number");
		return (1);
	}

	if (header->hmpl_endian != local_endianness)
	{
#ifdef __GNUC__ /* compiled with gcc, can use gcc builtins for fast assembly */
		header->hmpl_txs = __builtin_bswap32(header->hmpl_txs);
#else /* use function to manually byte swap */
		_swap_endian(&(header->hmpl_txs), 4);
#endif
	}
	return (0);
}


/**
 * mempool_deserialize - deserializes a blockchain mempool from a file; see
 *   mempool_serialize and blockchain_serialize for serialization format
 *
 * @path: full path to file containing serialized mempool
 *
 * Return: pointer to deserialized mempool, or NULL on failure
 */
llist_t *mempool_deserialize(char const *path)
{
	int fd;
	struct stat st;
	uint8_t local_endianness;
	mpl_file_hdr_t header;
	llist_t *mempool;

	if (!path)
	{
		fprintf(stderr, "mempool_deserialize: NULL parameter\n");
		return (NULL);
	}

	/* Can't use pathToReadFD(path) - mempool has different minimum size */
	if (lstat(path, &st) == -1)
	{
		fprintf(stderr, "mempool_deserialize: lstat '%s': %s\n", path,
			strerror(errno));
		return (NULL);
	}

	/* st.st_size is type off_t, or long int */
	if ((size_t)(st.st_size) < sizeof(mpl_file_hdr_t))
	{
		fprintf(stderr, "mempool_deserialize: %s\n",
			"file too small to contain header");
		return (NULL);
	}

	fd = open(path, O_RDONLY);
	if (fd == -1)
	{
		fprintf(stderr, "mempool_deserialize: open '%s': %s\n", path,
			strerror(errno));
	}

	local_endianness = _get_endianness();
	if (readMplFileHdr(fd, local_endianness, &header) != 0)
	{
		close(fd);
		return (NULL);
	}

	mempool = llist_create(MT_SUPPORT_FALSE);
	if (!mempool)
	{
		fprintf(stderr,
			"mempool_deserialize: llist_create: %s\n",
			strE_LLIST(llist_errno));
		close(fd);
		return (NULL);
	}

	if (readTransactions(fd, mempool, header.hmpl_txs, local_endianness,
			     header.hmpl_endian) != 0)
	{
		close(fd);
		llist_destroy(mempool, 1, (node_dtor_t)transaction_destroy);
		return (NULL);
	}

	close(fd);
	return (mempool);
}


/**
 * cmd_mempool_load - loads a mempool from a given path to use in the current
 *   CLI session
 *
 * @path: user provided path to the file from which to load the mempool,
 *   or NULL for the default
 * @arg2: dummy arg to conform to cmd_fp_t typedef
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 *
 * Return: 0 on success, 1 on failure
 */
int cmd_mempool_load(char *path, char *arg2, cli_state_t *cli_state)
{
	struct stat st;
	llist_t *new_mempool;

	(void)arg2;
	if (!cli_state)
	{
		fprintf(stderr, "cmd_mempool_load: NULL cli_state parameter\n");
		return (1);
	}

	if (!path || !path[0])
	{
		printf(TAB4 "No mempool file path provided, using default\n");
		path = MEMPOOL_PATH_DFLT;
	}

	if (lstat(path, &st) == -1)
	{
		printf(TAB4 "Failed to load mempool from '%s': %s\n",
		       path, strerror(errno));
		return (1);
	}
	else if (!S_ISREG(st.st_mode))
	{
		printf(TAB4 "Failed to load mempool from '%s': %s\n",
		       path, "Not a regular file");
		return (1);
	}

	new_mempool = mempool_deserialize(path);
	if (!new_mempool)
	{
		printf(TAB4 "Failed to load mempool from '%s'\n", path);
		return (1);
	}

	printf(TAB4 "Loaded mempool from '%s'\n", path);
	llist_destroy(cli_state->mempool, 1,
		      (node_dtor_t)transaction_destroy);
	cli_state->mempool = new_mempool;
	return (0);
}
