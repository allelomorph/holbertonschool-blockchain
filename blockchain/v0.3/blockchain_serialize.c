/* block_t blockchain_t strE_LLIST bc_file_hdr_t HBLK_MAG HBLK_VER */
#include "blockchain.h"
/* lstat `struct stat` S_ISDIR */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
/* errno ENOENT */
#include <errno.h>
/* fprintf perror write remove */
#include <stdio.h>
/* open O_* S_* close */
#include <fcntl.h>
/* _get_endianness */
#include "provided/endianness.h"
/* llist_* */
#include <llist.h>


/**
 * pathToWriteFD - validates a path and opens a file descriptor in
 *   write-only mode at that path
 *
 * @path: contains the potential path to a file to open
 *
 * Return: file descriptor upon success, or -1 upon failure
 */
int pathToWriteFD(char const *path)
{
	struct stat st;
	int fd;

	if (lstat(path, &st) == -1)
	{
		if (errno != ENOENT)
		{
			perror("pathToWriteFD: lstat");
			return (-1);
		}
	}
	else if (S_ISDIR(st.st_mode))
	{
		fprintf(stderr, "pathToWriteFD: path is a directory\n");
		return (-1);
	}

	fd = open(path, O_CREAT | O_TRUNC | O_WRONLY, /* 0664 */
		  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	if (fd == -1)
	{
		if (errno == ENOENT)
		{
			fprintf(stderr, "pathToWriteFD: %s\n",
				"some directory in path does not exist");
		}
		else
			perror("pathToWriteFD: open");
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
 * File Header format:
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
 *  0x0C   4       hblk_unspent Number of unspent transaction outputs in the
 *                                blockchain; endianness dependent
 *  0x10   ?       blocks       List of Blocks, see writeBlock
 *  0x10  165      unspent      List of unspent transaction outputs,
 *   + ?   * hblk_unspent         see writeUnspent
 *
 * Return: 0 on success, or 1 upon failure
 */
int writeBlkchnFileHdr(int fd, const blockchain_t *blockchain)
{
	bc_file_hdr_t header = { HBLK_MAG, HBLK_VER, 0, 0, 0 };
	int block_ct, unspent_ct;


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

	unspent_ct = llist_size(blockchain->unspent);
	if (unspent_ct == -1)
	{
		fprintf(stderr, "writeBlkchnFileHdr: llist_size: %s\n",
			strE_LLIST(llist_errno));
		return (1);
	}
	header.hblk_unspent = (uint32_t)unspent_ct;

	if (write(fd, &header, sizeof(bc_file_hdr_t)) == -1)
	{
		perror("writeBlkchnFileHdr: write");
		return (1);
	}

	return (0);
}


/**
 * writeBlock - used as `action` for llist_for_each to serialize and write to
 *   file a block in the blockchain->chain list
 *
 * @block: current block in blockchain->chain
 * @idx: index of block in blockchain->chain list
 * @fd: pointer to file descriptor open for writing
 *
 * Blocks are serialized contiguously, the first starting at file offset
 *   0x10, and adhere to the following format:
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
 *   + data_len
 *  0x5C   4        nb_transactions
 *   + data_len                Number of transactions in the block; endianness
 *                               dependant; -1 is for NULL (e.g. Genesis
 *                               block), 0 is for empty list
 *  0x60   ?        transactions
 *    + data_len               List of transactions, see writeTransaction
 *
 * Return: 0 on incremental success (llist_for_each can continue,)
 *   and -2 on failure (-1 reserved for llist_for_each errors)
 */
int writeBlock(block_t *block, unsigned int idx, int *fd)
{
	int32_t nb_transactions;

	if (!block || !fd)
	{
		fprintf(stderr, "writeBlock: NULL parameter(s)\n");
		return (-2);
	}

	/* transactions is NULL for Genesis block, encoded as -1 */
	nb_transactions = llist_size(block->transactions);
	if (nb_transactions == -1 &&
	    llist_errno != LLIST_NULL_ARGUMENT)
	{
		fprintf(stderr, "writeBlock: llist_size: %s\n",
			strE_LLIST(llist_errno));
		return (-2);
	}

	if (write(*fd, &(block->info), sizeof(block_info_t)) == -1 ||
	    write(*fd, &(block->data.len), sizeof(uint32_t)) == -1 ||
	    write(*fd, &(block->data.buffer), block->data.len) == -1 ||
	    write(*fd, &(block->hash), SHA256_DIGEST_LENGTH) == -1 ||
	    write(*fd, &nb_transactions, sizeof(uint32_t)) == -1)
	{
		perror("writeBlock: write");
		return (-2);
	}

	if (idx > 0 && block->info.index > 0 &&
	    llist_for_each(block->transactions,
			   (node_func_t)writeTransaction, fd) < 0)
	{
		fprintf(stderr,
			"writeBlock: llist_for_each failure\n");
		return (-2);
	}

	return (0);
}


/**
 * writeTransaction - used as `action` for llist_for_each to serialize and
 *   write to file a transaction in a block->transactions list
 *
 * @tx: current transaction in block->transactions
 * @idx: index of transaction in a block->transactions list
 * @fd: pointer to file descriptor open for writing
 *
 * Transaction serialized contiguously, the first starting at offset
 *    0x60 + data_len from the start of a serialized block:
 *  Offset Size(b)  Field      Purpose
 *  0x00   32       id         Transaction ID (hash)
 *  0x20   4        nb_inputs  Number of transaction inputs in the transaction;
 *                               endianness dependant
 *  0x24   4        nb_outputs Number of transaction outputs in the
 *                               transaction; endianness dependant
 *  0x28   169      inputs     List of transaction inputs, see writeInput
 *          * nb_inputs
 *  0x28   101      outputs    List of transaction outputs, see writeOutput
 *   +      * nb_outputs
 *   169 * nb_inputs
 *
 * Return: 0 on incremental success (llist_for_each can continue,)
 *   and -2 on failure (-1 reserved for llist_for_each errors)
 */
int writeTransaction(transaction_t *tx, unsigned int idx, int *fd)
{
	int nb_inputs, nb_outputs;

	(void)idx;
	if (!tx || !fd)
	{
		fprintf(stderr, "writeTransaction: NULL parameter(s)\n");
		return (-2);
	}

	nb_inputs = llist_size(tx->inputs);
	if (nb_inputs == -1)
	{
		fprintf(stderr, "writeTransaction: llist_size: %s\n",
			strE_LLIST(llist_errno));
		return (-2);
	}
	nb_outputs = llist_size(tx->outputs);
	if (nb_outputs == -1)
	{
		fprintf(stderr, "writeTransaction: llist_size: %s\n",
			strE_LLIST(llist_errno));
		return (-2);
	}

	if (write(*fd, &(tx->id), SHA256_DIGEST_LENGTH) == -1 ||
	    write(*fd, &nb_inputs, sizeof(uint32_t)) == -1 ||
	    write(*fd, &nb_outputs, sizeof(uint32_t)) == -1)
	{
		perror("writeTransaction: write");
		return (-2);
	}

	if (llist_for_each(tx->inputs,
			   (node_func_t)writeInput, fd) < 0)
	{
		fprintf(stderr,
			"writeTransaction: llist_for_each failure\n");
		return (-2);
	}

	if (llist_for_each(tx->outputs,
			   (node_func_t)writeOutput, fd) < 0)
	{
		fprintf(stderr,
			"writeTransaction: llist_for_each failure\n");
		return (-2);
	}

	return (0);
}


/**
 * writeInput - used as `action` for llist_for_each to serialize and
 *   write to file an input in a transaction->inputs list
 *
 * @tx_in: current input in transaction->inputs
 * @idx: index of input in a transaction->inputs list
 * @fd: pointer to file descriptor open for writing
 *
 * Transaction inputs serialized contiguously, the first starting at offset
 *    0x28 from start of transaction, each stored in exactly 169 bytes:
 *  Offset Size(b)  Field        Purpose
 *  0x00   32       block_hash   Hash of the block in which the consumed
 *                                 transaction output is located
 *  0x20   32       tx_id        ID (hash) of the transaction in which the
 *                                 consumed transaction output is located
 *  0x40   32       tx_out_hash  Hash of the consumed transaction output
 *  0x60   72       sig.sig      Transaction input signature buffer
 *  0xA8   1        sig.len      Transaction input signature length
 *
 * Return: 0 on incremental success (llist_for_each can continue,)
 *   and -2 on failure (-1 reserved for llist_for_each errors)
 */
int writeInput(tx_in_t *tx_in, unsigned int idx, int *fd)
{
	(void)idx;
	if (!tx_in || !fd)
	{
		fprintf(stderr, "writeInput: NULL parameter(s)\n");
		return (-2);
	}

	/*
	 * sizeof(tx_in_t):169 block_hash:32 tx_id:32 tx_out_hash:32
	 *   sig:sizeof(sig_t):73 sig72 len:1
	 * no padding observed, size of struct matches total of members
	 */
	if (write(*fd, tx_in, sizeof(tx_in_t)) == -1)
	{
		perror("writeInput: write");
		return (-2);
	}

	return (0);
}


/**
 * writeOutput - used as `action` for llist_for_each to serialize and
 *   write to file an output in a transaction->outputs list
 *
 * @tx_out: current output in transaction->outputs
 * @idx: index of output in a transaction->outputs list
 * @fd: pointer to file descriptor open for writing
 *
 * Transaction outputs serialized contiguously, the first starting after the
 *    last transaction input, each stored in exactly 101 bytes:
 *  Offset Size(b)  Field      Purpose
 *  0x00   4        amount     Transaction output amount; endianness dependant
 *  0x04   65       pub        Receiver’s public key
 *  0x45   32       hash       Transaction output hash
 *
 * Return: 0 on incremental success (llist_for_each can continue,)
 *   and -2 on failure (-1 reserved for llist_for_each errors)
 */
int writeOutput(tx_out_t *tx_out, unsigned int idx, int *fd)
{
	(void)idx;
	if (!tx_out || !fd)
	{
		fprintf(stderr, "writeOutput: NULL parameter(s)\n");
		return (-2);
	}

	/*
	 * sizeof(tx_out_t):104 amount:4 pub:65 hash:32
	 * padded with 3 bytes at end, must serialize manually to total 101
	 */
	if (write(*fd, &(tx_out->amount), sizeof(uint32_t)) == -1 ||
	    write(*fd, &(tx_out->pub), EC_PUB_LEN) == -1 ||
	    write(*fd, &(tx_out->hash), SHA256_DIGEST_LENGTH) == -1)
	{
		perror("writeOutput: write");
		return (-2);
	}

	return (0);
}


/**
 * writeUnspent - used as `action` for llist_for_each to serialize and
 *   write to file an unspent output in blockchain->unspent list
 *
 * @unspent_tx_out: current unspent output in blockchain->unspent
 * @idx: index of unspent output in a blockchain->unspent list
 * @fd: pointer to file descriptor open for writing
 *
 * Unspent transaction outputs serialized contiguously, the first starting
 *   after the last serialized block, each stored in exactly 165 bytes:
 *  Offset Size(b)  Field       Purpose
 *  0x00   32       block_hash  Hash of the block the referenced transaction
 *                                output belongs to
 *  0x20   32       tx_id       ID of the transaction the referenced
 *                                transaction output belongs to
 *  0x40   4        out.amount  Amount of the referenced transaction output,
 *                                endianness dependant
 *  0x44   65       out.pub     Receiver’s public key of the referenced
 *                                transaction output
 *  0x85   32       out.hash    Hash of the referenced transaction output
 *
 * Return: 0 on incremental success (llist_for_each can continue,)
 *   and -2 on failure (-1 reserved for llist_for_each errors)
 */
int writeUnspent(unspent_tx_out_t *unspent_tx_out, unsigned int idx, int *fd)
{
	(void)idx;
	if (!unspent_tx_out || !fd)
	{
		fprintf(stderr, "writeUnspent: NULL parameter(s)\n");
		return (-2);
	}

	/*
	 * sizeof(unspent_tx_out_t):168 block_hash:32 tx_id:32
	 *   out.amount:4 out.pub:65 out.hash:32
	 * (3 bytes of padding at end of tx_out_t requires serializing
	 *   manually to total 165)
	 */
	if (write(*fd, &(unspent_tx_out->block_hash),
		  SHA256_DIGEST_LENGTH) == -1 ||
	    write(*fd, &(unspent_tx_out->tx_id), SHA256_DIGEST_LENGTH) == -1 ||
	    write(*fd, &(unspent_tx_out->out.amount),
		  sizeof(uint32_t)) == -1 ||
	    write(*fd, &(unspent_tx_out->out.pub), EC_PUB_LEN) == -1 ||
	    write(*fd, &(unspent_tx_out->out.hash), SHA256_DIGEST_LENGTH) == -1)
	{
		perror("writeUnspent: write");
		return (-2);
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
 *  File header (see writeBlkchnFileHdr)
 *  Blocks (see writeBlock,) with each block containing:
 *      Block info and data (see writeBlock)
 *      Number of Transactions (see  writeBlock)
 *      Transactions (see writeTransaction,) with each transaction
 *       containing:
 *          Number of inputs and outputs
 *          Inputs (see writeInput)
 *          Outputs (see writeOutput)
 *  Unspent outputs (see writeUnspent)
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

	fd = pathToWriteFD(path);
	if (fd == -1)
		return (-1);

	if (writeBlkchnFileHdr(fd, blockchain) != 0)
	{
		close(fd);
		remove(path);
		return (-1);
	}

	if (llist_for_each(blockchain->chain,
			   (node_func_t)writeBlock, &fd) < 0)
	{
		fprintf(stderr,
			"blockchain_serialize: llist_for_each failure\n");
		close(fd);
		remove(path);
		return (-1);
	}

	if (llist_for_each(blockchain->unspent,
			   (node_func_t)writeUnspent, &fd) < 0)
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
