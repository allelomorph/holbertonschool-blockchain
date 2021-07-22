CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic -std=gnu90 -ggdb3 -c

SRC = sha256.c ec_create.c ec_to_pub.c ec_from_pub.c \
	ec_save.c ec_load.c ec_sign.c ec_verify.c OpenSSLGlobalCleanup.c
OBJ = $(SRC:.c=.o)

.PHONY: libhblk_crypto.a clean oclean flcean

# ar -s includes object file index
libhblk_crypto.a: hblk_crypto.h $(OBJ)
	rm -f libhblk_crypto.a
	ar -rcs libhblk_crypto.a hblk_crypto.h $(OBJ)

clean:
	$(RM) *~ $(NAME)

oclean:
	$(RM) $(OBJ)

fclean: clean oclean
