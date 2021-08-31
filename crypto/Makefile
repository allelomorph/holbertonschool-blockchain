# (443) 0x00. Blockchain - Crypto

# note: OpenSSLGlobalCleanup() is provided as a ((destructor)) function to
#   de-allocate OpenSSL's internal tables on exit. When linking to
#   libhblk_crypto.a during compilation with gcc, use `-u OpenSSLGlobalCleanup`
#   to ensure it is seen by the linker and include this feature.
#
#   example: `gcc main.c -L. -lhblk_crypto -u OpenSSLGlobalCleanup`

CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic

# OpenSSLGlobalCleanup.c temporarily omitted for project grading of task 8
SRC = sha256.c \
	ec_create.c \
	ec_to_pub.c \
	ec_from_pub.c \
	ec_save.c \
	ec_load.c \
	ec_sign.c \
	ec_verify.c \
	OpenSSLGlobalCleanup.c

OBJ = $(SRC:.c=.o)

.PHONY: libhblk_crypto.a clean oclean flcean

# `ar -s` includes object file index, equivalent to ranlib (view with `ar -t`)
libhblk_crypto.a: $(OBJ)
	ar -rcs libhblk_crypto.a $(OBJ)

all: libhblk_crypto.a

clean:
	$(RM) *~ libhblk_crypto.a

oclean:
	$(RM) $(OBJ)

fclean: clean oclean
