/* block_t blockchain_t strE_LLIST bc_file_hdr_t HBLK_MAG* HBLK_VER* */
#include "blockchain.h"
/* lstat `struct stat` */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
/* errno */
#include <errno.h>
/* fprintf perror read */
#include <stdio.h>
/* open O_* close */
#include <fcntl.h>
/* _get_endianness _swap_endian */
#include "provided/endianness.h"
/* llist_* */
#include <llist.h>
/* strerror memcmp */
#include <string.h>


/**
 * pathToReadFD - validates a path and opens a file descriptor in
 *   read-only mode at that path
 *
 * @path: contains the potential path to a file to open
 *
 * Return: file descriptor upon success, or -1 upon failure
 */
int pathToReadFD(char const *path)
{
	struct stat st;
	int fd;

	if (lstat(path, &st) == -1)
	{
		fprintf(stderr, "pathToReadFD: lstat '%s': %s\n", path,
			strerror(errno));
		return (-1);
	}

	/* st.st_size is type off_t, or long int */
	if ((size_t)(st.st_size) < sizeof(bc_file_hdr_t) + GEN_BLK_SERIAL_SZ)
	{
		fprintf(stderr, "pathToReadFD: %s\n",
			"file too small to contain header and Genesis Block");
		return (-1);
	}

	fd = open(path, O_RDONLY);
	if (fd == -1)
	{
		fprintf(stderr, "pathToReadFD: open '%s': %s\n", path,
			strerror(errno));
	}

	return (fd);
}


/**
 * readBlkchnFileHdr - reads and validates a serialized file header from a
 *   file storing a blockchain
 *
 * @fd: file descriptor already open for reading
 * @local_endianness: 1 for little endian, 2 for big endian
 * @header: stores values read from file header
 *
 * Return: 0 on success, or 1 upon failure
 */
int readBlkchnFileHdr(int fd, uint8_t local_endianness,
		      bc_file_hdr_t *header)
{
	if (!header)
	{
		fprintf(stderr, "readBlkchnFileHdr: NULL parameter\n");
		return (1);
	}
	if (local_endianness != 1 && local_endianness != 2)
	{
		fprintf(stderr,
			"readBlkchnFileHdr: invalid local_endianness\n");
		return (1);
	}
	if (read(fd, header, sizeof(bc_file_hdr_t)) == -1)
	{
		perror("readBlkchnFileHdr: read");
		return (1);
	}
	if (memcmp(&(header->hblk_magic), HBLK_MAG, HBLK_MAG_LEN) != 0)
	{
		fprintf(stderr,
			"readBlkchnFileHdr: invalid magic number\n");
		return (1);
	}
	if (memcmp(&(header->hblk_version), HBLK_VER, HBLK_VER_LEN) != 0)
	{
		fprintf(stderr, "readBlkchnFileHdr: %s\n",
			"serialized with incompatible version number");
		return (1);
	}

	if (header->hblk_endian != local_endianness)
	{
#ifdef __GNUC__ /* compiled with gcc, can use gcc builtins for fast assembly */
		header->hblk_blocks = __builtin_bswap32(header->hblk_blocks);
		header->hblk_unspent = __builtin_bswap32(header->hblk_unspent);
#else /* use function to manually byte swap */
		_swap_endian(&(header->hblk_blocks), 4);
		_swap_endian(&(header->hblk_unspent), 4);
#endif
	}
	return (0);
}


/**
 * bswapBlock - reverses endianness of values found in or needed for
 *   deserialization of a block_t struct
 *
 * @block: pointer to the block to have values' byte order reversed
 * @nb_transactions: deserialized immediately following a block, modifed
 *   by reference
 *
 * Note: __builtin_bswapXX preferred over _swap_endian in
 *   provided/_endianness.c due to its efficiency, being compiled as only
 *   one assembly instruction.
 */
void bswapBlock(block_t *block, int32_t *nb_transactions)
{
	if (!block || !nb_transactions)
	{
		fprintf(stderr, "bswapBlock: NULL parameter(s)\n");
		return;
	}

#ifdef __GNUC__ /* compiled with gcc, can use gcc builtins for fast assembly */
	block->info.index      = __builtin_bswap32(block->info.index);
	block->info.difficulty = __builtin_bswap32(block->info.difficulty);
	block->info.timestamp  = __builtin_bswap64(block->info.timestamp);
	block->info.nonce      = __builtin_bswap64(block->info.nonce);
	block->data.len        = __builtin_bswap32(block->data.len);

	*nb_transactions       = __builtin_bswap32(*nb_transactions);
#else /* use function to manually byte swap */
	_swap_endian(&(block->info.index), 4);
	_swap_endian(&(block->info.difficulty), 4);
	_swap_endian(&(block->info.timestamp), 8);
	_swap_endian(&(block->info.nonce), 8);
	_swap_endian(&(block->data.len), 4);

	_swap_endian(nb_transactions, 4);
#endif
}


/**
 * readBlocks - reads serialized blocks from a storage file into a blockchain
 *   data structure
 *
 * @fd: file descriptor already open for reading
 * @chain: pointer to an empty list of blocks (Genesis Block removed)
 *   to contain the deserialized blocks
 * @local_endianness: 1 for little endian, 2 for big endian
 * @header: stores values read from file header
 *
 * Return: 0 on success, or 1 upon failure
 */
int readBlocks(int fd, llist_t *chain, uint8_t local_endianness,
	       bc_file_hdr_t *header)
{
	uint32_t i;
	int32_t nb_transactions;
	block_t *block;

	if (!chain || !header)
	{
		fprintf(stderr, "readBlocks: NULL parameter\n");
		return (1);
	}
	if (!llist_is_empty(chain))
	{
		fprintf(stderr, "readBlocks: target blockchain not empty\n");
		return (1);
	}
	for (i = 0; i < header->hblk_blocks; i++)
	{
		block = calloc(1, sizeof(block_t));
		if (!block)
		{
			fprintf(stderr, "readBlocks: calloc failure\n");
			return (1);
		}
		if (read(fd, &(block->info), sizeof(block_info_t)) == -1 ||
		    read(fd, &(block->data.len), sizeof(uint32_t)) == -1 ||
		    read(fd, &(block->data.buffer), block->data.len) == -1 ||
		    read(fd, &(block->hash), SHA256_DIGEST_LENGTH) == -1 ||
		    read(fd, &nb_transactions, sizeof(int32_t)) == -1)
		{
			perror("readBlocks: read");
			return (1);
		}
		if (local_endianness != header->hblk_endian)
			bswapBlock(block, &nb_transactions);

		if (nb_transactions == -1) /* genesis block */
			block->transactions = NULL;
		else
		{
			block->transactions = llist_create(MT_SUPPORT_FALSE);
			if (!block->transactions)
			{
				fprintf(stderr,
					"readBlocks: llist_create failure\n");
				return (1);
			}
			if (readTransactions(fd, block->transactions,
					     nb_transactions, local_endianness,
					     header->hblk_endian) != 0)
			{
				fprintf(stderr,
					"readBlocks: readTransactions failure\n");
				return (1);
			}
		}

		if (llist_add_node(chain, (llist_node_t)block,
				   ADD_NODE_REAR) != 0)
		{
			fprintf(stderr, "readBlocks: llist_add_node: %s\n",
				strE_LLIST(llist_errno));
			return (1);
		}
	}
	return (0);
}


/**
 * readTransactions - reads serialized transactions from a storage file into a
 *   blockchain data structure
 *
 * @fd: file descriptor already open for reading
 * @transactions: pointer to an empty transactions list to contain the
 *   deserialized transactions
 * @nb_transactions: amount of transactions to expect to read for this block
 * @local_endianness: 1 for little endian, 2 for big endian
 * @hblk_endian: 1 for little endian, 2 for big endian
 *
 * Return: 0 on success, or 1 upon failure
 */
int readTransactions(int fd, llist_t *transactions,
		     uint32_t nb_transactions, uint8_t local_endianness,
		     uint8_t hblk_endian)
{
	uint32_t i, nb_inputs, nb_outputs;
	transaction_t *tx;

	if (!transactions)
	{
		fprintf(stderr, "readTransactions: NULL parameter\n");
		return (1);
	}
	if (!llist_is_empty(transactions))
	{
		fprintf(stderr,
			"readTransactions: transaction list not empty\n");
		return (1);
	}
	for (i = 0; i < nb_transactions; i++)
	{
		tx = calloc(1, sizeof(transaction_t));
		if (!tx)
		{
			fprintf(stderr, "readTransactions: calloc failure\n");
			return (1);
		}
		if (read(fd, &(tx->id), SHA256_DIGEST_LENGTH) == -1 ||
		    read(fd, &nb_inputs, sizeof(uint32_t)) == -1 ||
		    read(fd, &nb_outputs, sizeof(uint32_t)) == -1)
		{
			perror("readTransactions: read");
			return (1);
		}
		if (local_endianness != hblk_endian)
		{
#ifdef __GNUC__ /* compiled with gcc, can use gcc builtins for fast assembly */
			nb_inputs = __builtin_bswap32(nb_inputs);
			nb_outputs = __builtin_bswap32(nb_outputs);
#else /* use function to manually byte swap */
			_swap_endian(&nb_inputs, 4);
			_swap_endian(&nb_outputs, 4);
#endif
		}

		tx->inputs = llist_create(MT_SUPPORT_FALSE);
		tx->outputs = llist_create(MT_SUPPORT_FALSE);
		if (!tx->inputs || !tx->outputs)
		{
			fprintf(stderr,
				"readTransactions: llist_create failure\n");
			return (1);
		}
		if (readInputs(fd, tx->inputs, nb_inputs) != 0)
		{
			fprintf(stderr,
				"readTransactions: readInputs failure\n");
			return (1);
		}
		if (readOutputs(fd, tx->outputs, nb_outputs,
				local_endianness, hblk_endian) != 0)
		{
			fprintf(stderr,
				"readTransactions: readOutputs failure\n");
			return (1);
		}

		if (llist_add_node(transactions, (llist_node_t)tx,
				   ADD_NODE_REAR) != 0)
		{
			fprintf(stderr,
				"readTransactions: llist_add_node: %s\n",
				strE_LLIST(llist_errno));
			return (1);
		}
	}
	return (0);
}


/**
 * readInputs - reads serialized transaction inputs from a storage file into a
 *   blockchain data structure
 *
 * @fd: file descriptor already open for reading
 * @inputs: pointer to an empty list of transaction inputs to contain the
 *   deserialized inputs
 * @nb_inputs: number of inputs to expect to read
 *
 * Return: 0 on success, or 1 upon failure
 */
int readInputs(int fd, llist_t *inputs, uint32_t nb_inputs)
{
	uint32_t i;
	tx_in_t *tx_in;

	if (!inputs)
	{
		fprintf(stderr, "readInputs: NULL parameter\n");
		return (1);
	}
	if (!llist_is_empty(inputs))
	{
		fprintf(stderr,
			"readInputs: inputs list not empty\n");
		return (1);
	}
	for (i = 0; i < nb_inputs; i++)
	{
		tx_in = calloc(1, sizeof(tx_in_t));
		if (!tx_in)
		{
			fprintf(stderr, "readInputs: calloc failure\n");
			return (1);
		}

		if (read(fd, tx_in, sizeof(tx_in_t)) == -1)
		{
			perror("readInputs: read");
			return (1);
		}

		if (llist_add_node(inputs, (llist_node_t)tx_in,
				   ADD_NODE_REAR) != 0)
		{
			fprintf(stderr,
				"readInputs: llist_add_node: %s\n",
				strE_LLIST(llist_errno));
			return (1);
		}
	}
	return (0);
}


/**
 * readOutputs - reads serialized transaction outputs from a storage file into
 *   a blockchain data structure
 *
 * @fd: file descriptor already open for reading
 * @outputs: pointer to an empty transaction outputs list to contain the
 *   deserialized outputs
 * @nb_outputs: number of outputs to expect to read
 * @local_endianness: 1 for little endian, 2 for big endian
 * @hblk_endian: 1 for little endian, 2 for big endian
 *
 * Return: 0 on success, or 1 upon failure
 */
int readOutputs(int fd, llist_t *outputs, uint32_t nb_outputs,
		uint8_t local_endianness, uint8_t hblk_endian)
{
	uint32_t i;
	tx_out_t *tx_out;

	if (!outputs)
	{
		fprintf(stderr, "readOutputs: NULL parameter\n");
		return (1);
	}
	if (!llist_is_empty(outputs))
	{
		fprintf(stderr,
			"readOutputs: output list not empty\n");
		return (1);
	}
	for (i = 0; i < nb_outputs; i++)
	{
		tx_out = calloc(1, sizeof(tx_out_t));
		if (!tx_out)
		{
			fprintf(stderr, "readOutputs: calloc failure\n");
			return (1);
		}

		/* serialized one member at a time, see blockchain_serialize */
		if (read(fd, &(tx_out->amount), sizeof(uint32_t)) == -1 ||
		    read(fd, &(tx_out->pub), EC_PUB_LEN) == -1 ||
		    read(fd, &(tx_out->hash), SHA256_DIGEST_LENGTH) == -1)
		{
			perror("readOutputs: read");
			return (1);
		}

		if (local_endianness != hblk_endian)
#ifdef __GNUC__ /* compiled with gcc, can use gcc builtins for fast assembly */
			tx_out->amount = __builtin_bswap32(tx_out->amount);
#else /* use function to manually byte swap */
			_swap_endian(&(tx_out->amount), 4);
#endif

		if (llist_add_node(outputs, (llist_node_t)tx_out,
				   ADD_NODE_REAR) != 0)
		{
			fprintf(stderr,
				"readOutputs: llist_add_node: %s\n",
				strE_LLIST(llist_errno));
			return (1);
		}
	}
	return (0);
}


/**
 * readUnspent - reads serialized unspent outputs from a storage file into a
 *   blockchain data structure
 *
 * @fd: file descriptor already open for reading
 * @unspent: pointer to an empty unspent output list to contain the
 *   deserialized unspent outputs
 * @local_endianness: 1 for little endian, 2 for big endian
 * @header: stores values read from file header
 *
 * Return: 0 on success, or 1 upon failure
 */
int readUnspent(int fd, llist_t *unspent,
		uint8_t local_endianness, bc_file_hdr_t *header)
{
	uint32_t i;
	unspent_tx_out_t *unspent_tx_out;

	if (!unspent || !header)
	{
		fprintf(stderr, "readUnspent: NULL parameter(s)\n");
		return (1);
	}
	if (!llist_is_empty(unspent))
	{
		fprintf(stderr,
			"readUnspent: unspent output list not empty\n");
		return (1);
	}
	for (i = 0; i < header->hblk_unspent; i++)
	{
		unspent_tx_out = calloc(1, sizeof(unspent_tx_out_t));
		if (!unspent_tx_out)
		{
			fprintf(stderr, "readUnspent: calloc failure\n");
			return (1);
		}

		/* serialized one member at a time, see blockchain_serialize */
		if (read(fd, &(unspent_tx_out->block_hash),
			 SHA256_DIGEST_LENGTH) == -1 ||
		    read(fd, &(unspent_tx_out->tx_id),
			 SHA256_DIGEST_LENGTH) == -1 ||
		    read(fd, &(unspent_tx_out->out.amount),
			  sizeof(uint32_t)) == -1 ||
		    read(fd, &(unspent_tx_out->out.pub),
			 EC_PUB_LEN) == -1 ||
		    read(fd, &(unspent_tx_out->out.hash),
			 SHA256_DIGEST_LENGTH) == -1)
		{
			perror("readUnspent: read");
			return (1);
		}

		if (local_endianness != header->hblk_endian)
		{
#ifdef __GNUC__ /* compiled with gcc, can use gcc builtins for fast assembly */
			unspent_tx_out->out.amount =
				__builtin_bswap32(unspent_tx_out->out.amount);
#else /* use function to manually byte swap */
			_swap_endian(&(unspent_tx_out->out.amount), 4);
#endif
		}

		if (llist_add_node(unspent, (llist_node_t)unspent_tx_out,
				   ADD_NODE_REAR) != 0)
		{
			fprintf(stderr,
				"readUnspent: llist_add_node: %s\n",
				strE_LLIST(llist_errno));
			return (1);
		}
	}
	return (0);
}


/**
 * blockchain_deserialize - deserializes a blockchain from a file; see
 *   blockchain_serialize for serialization format
 *
 * @path: full path to file containing serialized blockchain
 *
 * Return: pointer to deserialized blockchain, or NULL on failure
 */
blockchain_t *blockchain_deserialize(char const *path)
{
	int fd;
	uint8_t local_endianness;
	bc_file_hdr_t header;
	blockchain_t *blockchain;
	block_t *genesis;

	if (!path)
	{
		fprintf(stderr, "blockchain_deserialize: NULL parameter\n");
		return (NULL);
	}

	fd = pathToReadFD(path);
	if (fd == -1)
		return (NULL);
	blockchain = blockchain_create();
	if (!blockchain)
	{
		fprintf(stderr,
			"blockchain_deserialize: blockchain_create failure\n");
		close(fd);
		return (NULL);
	}
	/* remove preloaded Genesis Block at head of list */
	genesis = (block_t *)llist_pop(blockchain->chain);
	if (genesis)
		free(genesis);
	local_endianness = _get_endianness();

	if (readBlkchnFileHdr(fd, local_endianness, &header) != 0 ||
	    readBlocks(fd, blockchain->chain, local_endianness, &header) != 0 ||
	    readUnspent(fd, blockchain->unspent,
			local_endianness, &header) != 0)
	{
		close(fd);
		blockchain_destroy(blockchain);
		return (NULL);
	}

	close(fd);
	return (blockchain);
}
