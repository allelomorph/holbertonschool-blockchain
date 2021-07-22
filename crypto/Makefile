# (443) 0x00. Blockchain - Crypto

# note: When linking to libhblk_crypto during compilation with gcc, use
#   `-u` to set OpenSSLGlobalCleanup` in .dtors to free OpenSSL internal tables.
#   example: `gcc main.c -L. -lhblk_crypto -u OpenSSLGlobalCleanup`

CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic -c

# OpenSSLGlobalCleanup.c temporarily omitted for project grading of task 8
SRC = sha256.c ec_create.c ec_to_pub.c ec_from_pub.c \
	ec_save.c ec_load.c ec_sign.c ec_verify.c # OpenSSLGlobalCleanup.c
OBJ = $(SRC:.c=.o)

.PHONY: libhblk_crypto.a clean oclean flcean

# `ar -s` includes object file index, equivalent to ranlib (view with `ar -t`)
libhblk_crypto.a: hblk_crypto.h $(OBJ)
	ar -rcs libhblk_crypto.a $(OBJ)

clean:
	$(RM) *~ $(NAME)

oclean:
	$(RM) $(OBJ)

fclean: clean oclean
