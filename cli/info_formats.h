#ifndef INFO_FORMATS_H
#define INFO_FORMATS_H

/* TAB4  RULER */
#include "hblk_cli.h"

/*
 * 0x04 t4 requirements:
 *
 *  Display the number of Blocks in the Blockchain
 *  Display the number of unspent transaction output
 *  Display the number of pending transactions in the local transaction pool
 */

#define INFO_WALLET_FMT_HDR "\n" \
	TAB4 "Current status of session wallet:\n"

#define INFO_MEMPOOL_FMT_HDR "\n" \
	TAB4 "Current status of session mempool:\n"

#define INFO_BLKCHN_FMT_HDR "\n" \
	TAB4 "Current status of session blockchain:\n"

#define INFO_FMT_FTR "\n"

#define INFO_WALLET_FMT \
	TAB4 TAB4 "* public key: %s\n" \
	TAB4 TAB4 "* number of unspent outputs (UTXOs): %i\n" \
	TAB4 TAB4 "* total coin of unspent outputs (UTXOs): %u\n"

#define UNSPENT_CACHE_INTRO \
	TAB4 "Note: UTXOs listed are those confirmed in previous blocks " \
	"minus those\nreferenced as inputs in current mempool transactions.\n"

#define INFO_MEMPOOL_FMT \
	TAB4 TAB4 "* total transactions in mempool awaiting confirmation: %i\n"

#define INFO_BLKCHN_FMT \
	TAB4 TAB4 "* total unspent outputs:\n" \
	TAB4 TAB4 TAB4 "* (before mempool transactions are confirmed): %i\n" \
	TAB4 TAB4 TAB4 "* (after mempool transactions are confirmed): %i\n" \
	TAB4 TAB4 "* blockchain height (block total incl. Genesis Block): %i\n"


#endif /* INFO_FORMATS_H */
