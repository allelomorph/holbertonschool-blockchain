# -*- MakeFile -*-
# (459) 0x04. Blockchain - CLI
#
# Compiles all source files in v0.3 Holberton blockchain CLI implementation.
#
#  Prerequisites:
#   - OpenSSL 1.0.1f (libssl.so and libcrypto.so)
#   - libssl-dev (/usr/include/openssl/*.h)
#   - libllist.so, llist.h: provided for this project, found at
#       https://github.com/holbertonschool/holbertonschool-blockchain/tree/master/llist
#   - libhblk_crypto.a: created in (443) 0x00. Blockchain - Crypto, found in
#       ../crypto
#   - libhblk_blockchain.a: created in (458) 0x03. Blockchain - Transactions,
#       found in ../blockchain/v0.3/
#
#  Notes:
#   - Tested with Holberton standard of gcc 4.8.4, which defaults to C90:
#       `-std=gnu90`, which along with the strict `-W`... and `-pedantic` flags,
#       throws errors for string constants over 509 characters. This is avoided
#       by using `-Wno-overlength-strings` to turn off the implicitly included
#       `-Woverlength-strings`. `-std=gnu99` would also work, as in C99, the
#       limit was raised to 4095.
#   - OpenSSLGlobalCleanup is an __attribute__ ((destructor)) function in
#       libhblk_crypto.a which deallocates internal OpenSSL tables after main
#	exits; as an implicitly called destructor we must force the linker to
#       treat it as undefined.
#
#  Implicit make rules used:
#   - "n.c" compiled with `$(CC) -c $(CPPFLAGS) $(CFLAGS)`
#
#  Latest manual version: `gcc -Wall -Werror -Wextra -pedantic -I. -I../blockchain/v0.3/ -I../blockchain/v0.3/transaction/ -I../crypto/ -o hblk_cli hblk_cli.* cli_loop.c scripts.c lexing.c cleanup.c cmd_*.c printing/*.c -L../crypto/ -L../blockchain/v0.3/ -u OpenSSLGlobalCleanup -lhblk_blockchain -lhblk_crypto -lssl -lcrypto -lllist -pthread -Wno-overlength-strings`

CC = gcc
MAKE = make
RM = rm -f

HDR_DIRS = . ../blockchain/v0.3/ ../blockchain/v0.3/transaction/ ../crypto/
SILENCE = -Wno-overlength-strings
CFLAGS = -Wall -Werror -Wextra -pedantic $(SILENCE) $(addprefix -I, $(HDR_DIRS))

LIB_DIRS = ../crypto/ ../blockchain/v0.3/
LDFLAGS = -u OpenSSLGlobalCleanup $(addprefix -I, $(HDR_DIRS)) \
	$(addprefix -L, $(LIB_DIRS))

LNKD_LIBS = hblk_blockchain hblk_crypto ssl crypto llist
LDLIBS = $(addprefix -l, $(LNKD_LIBS)) -pthread

HEADERS = hblk_cli.h help_text.h info_formats.h
CLI_SRC = cleanup.c \
	cli_loop.c \
	cmd_exit.c \
	cmd_help.c \
	cmd_info.c \
	cmd_load.c \
	cmd_mempool_load.c \
	cmd_mempool_save.c \
	cmd_mine.c \
	cmd_new.c \
	cmd_save.c \
	cmd_send.c \
	cmd_wallet_load.c \
	cmd_wallet_save.c \
	hblk_cli.c \
	lexing.c \
	scripts.c
PRINT_SRC = _blockchain_print.c \
	_print_all_unspent.c \
	_print_hex_buffer.c \
	_transaction_print_brief.c \
	_transaction_print.c

OBJ = $(CLI_SRC:.c=.o) $(addprefix printing/, $(PRINT_SRC:.c=.o))

.PHONY: llist all clean oclean fclean re

hblk_cli: /usr/local/lib/libllist.so /usr/local/include/llist.h \
	../crypto/libhblk_crypto.a ../blockchain/v0.3/libhblk_blockchain.a \
	$(HEADERS) $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

../crypto/libhblk_crypto.a:
	$(MAKE) -C ../crypto/ libhblk_crypto.a oclean

../blockchain/v0.3/libhblk_blockchain.a:
	$(MAKE) -C ../blockchain/v0.3/ libhblk_blockchain.a oclean

/usr/local/lib/libllist.so:
/usr/local/include/llist.h:
	cd ../llist/ && sudo ./install.bash && cd -

all: hblk_cli

clean:
	$(RM) *~ hblk_cli

oclean:
	$(RM) $(OBJ)

fclean: clean oclean

re: oclean all
