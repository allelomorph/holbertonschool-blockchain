#include "blockchain.h"


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
}
