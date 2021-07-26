# (443) 0x00. Blockchain - Crypto
Specializations > System programming & Algorithm > Blockchain

---

### Project author
Alexandre Gautier

### Assignment dates
07-14-2021 to 07-23-2021

### Description
Introduction to Blockchains and their encryption, including concepts like hash algorithms (SHA in particular,) Elliptic Curve Cryptography (ECC,) and Elliptic Curve Digital Signature Algorithm (ECDSA;) all in the context of using the OpenSSL C API.


### Provided file(s)
* [_print_hex_buffer.c](./provided/_print_hex_buffer.c)
* [ec_create-main.c](./test/ec_create-main.c)
* [ec_from_pub-main.c](./test/ec_from_pub-main.c)
* [ec_load-main.c](./test/ec_load-main.c)
* [ec_save-main.c](./test/ec_save-main.c)
* [ec_sign-main.c](./test/ec_sign-main.c)
* [ec_to_pub-main.c](./test/ec_to_pub-main.c)
* [ec_verify-main.c](./test/ec_verify-main.c)
* [sha256-main.c](./test/sha256-main.c)

---

## Mandatory Tasks

### :white_check_mark: 0. SHA256
Write a function that computes the hash of a sequence of bytes

* Prototype: `uint8_t *sha256(int8_t const *s, size_t len, uint8_t digest[SHA256_DIGEST_LENGTH]);`, where
    * `s` is the sequence of bytes to be hashed
    * `len` is the number of bytes to hash in `s`
* The resulting hash must be stored in `digest`
* Your function must return a pointer to `digest`
* If `digest` happens to be `NULL`, your function must do nothing and return `NULL`
* You are not allowed to allocate memory dynamically

File(s): [`sha256.c`](./sha256.c) [`hblk_crypto.h`](./hblk_crypto.h)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -I. -o sha256-test test/sha256-main.c provided/_print_hex_buffer.c sha256.c -lssl -lcrypto`

### :white_check_mark: 1. EC_KEY creation
Write a function that creates a new EC key pair.

* Prototype: `EC_KEY *ec_create(void);`
* Your function must return a pointer to an `EC_KEY` structure, containing both the public and private keys, or `NULL` upon failure
* Both the private and the public keys must be generated
* You have to use the [secp256k1](https://en.bitcoin.it/wiki/Secp256k1) elliptic curve to create the new pair (See `EC_CURVE` macro)

File(s): [`ec_create.c`](./ec_create.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -I. -o ec_create-test test/ec_create-main.c ec_create.c -lssl -lcrypto`

### :white_check_mark: 2. EC_KEY to public key
Write a function that extracts the public key from an `EC_KEY` opaque structure

* Prototype: `uint8_t *ec_to_pub(EC_KEY const *key, uint8_t pub[EC_PUB_LEN]);`, where:
    * `key` is a pointer to the `EC_KEY` structure to retrieve the public key from. If it is `NULL`, your function must do nothing and fail
    * `pub` is the address at which to store the extracted public key (The key shall not be compressed)
* Your function must return a pointer to `pub`
* `NULL` must be returned upon failure, and there should not be any memory leak

*NOTE*: It is also possible to extract the private key from an `EC_KEY` structure, but we’re never going to store one’s private key anywhere in the Blockchain. So we don’t really need it.

File(s): [`ec_to_pub.c`](./ec_to_pub.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -I. -o ec_to_pub-test test/ec_to_pub-main.c provided/_print_hex_buffer.c ec_to_pub.c ec_create.c -lssl -lcrypto`

### :white_check_mark: 3. EC_KEY from public key
Write a function that creates an `EC_KEY` structure given a public key

* Prototype: `EC_KEY *ec_from_pub(uint8_t const pub[EC_PUB_LEN]);`, where:
    * `pub` contains the public key to be converted
* Your function must return a pointer to the created `EC_KEY` structure upon success, or `NULL` upon failure
* The created `EC_KEY`‘s private key does not have to be initialized/set, we only care about the public one

File(s): [`ec_from_pub.c`](./ec_from_pub.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -I. -o ec_from_pub-test test/ec_from_pub-main.c provided/_print_hex_buffer.c ec_from_pub.c ec_to_pub.c ec_create.c -lssl -lcrypto`

### :white_check_mark: 4. EC_KEY - Save to file
Write a function that saves an existing EC key pair on the disk.

* Prototype: `int ec_save(EC_KEY *key, char const *folder);`, where
    * `key` points to the EC key pair to be saved on disk
    * `folder` is the path to the folder in which to save the keys (e.g. `/home/hblk/alex`)
* Your function must respectively return 1 or 0 upon success or failure
* `folder` must be created if it doesn’t already exist
    * `<folder>/key.pem` will contain the **private** key, in the **PEM** format. The file must be created, or overridden if it already exists (e.g. `/home/alex/.hblk/alex/key.pem`)
    * `<folder>/key_pub.pem` will contain the **public** key, in the **PEM** format. The file must be created, or overridden if it already exists (e.g. `/home/alex/.hblk/alex/key_pub.pem`)

File(s): [`ec_save.c`](./ec_save.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -I. -o ec_save-test test/ec_save-main.c ec_save.c ec_create.c ec_to_pub.c provided/_print_hex_buffer.c -lssl -lcrypto`

### :white_check_mark: 5. EC_KEY - Load from file
Write a function that loads an EC key pair from the disk.

* Prototype: `EC_KEY *ec_load(char const *folder);`, where
    * `folder` is the path to the folder from which to load the keys (e.g. `/home/hblk/alex`)
* Your function must return a pointer to the created EC key pair upon success, or `NULL` upon failure
* From the folder `folder`:
    * `<folder>/key.pem` will contain the **private** key, in the **PEM** format.
    * `<folder>/key_pub.pem` will contain the **public** key, in the **PEM** format.

File(s): [`ec_load.c`](./ec_load.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -I. -o ec_load-test test/ec_load-main.c ec_load.c ec_save.c ec_create.c ec_to_pub.c provided/_print_hex_buffer.c -lssl -lcrypto`

### :white_check_mark: 6. Signature
Write a function that signs a given set of bytes, using a given EC_KEY **private key**

* Prototype: `uint8_t *ec_sign(EC_KEY const *key, uint8_t const *msg, size_t msglen, sig_t *sig);`, where:
    * `key` points to the `EC_KEY` structure containing the private key to be used to perform the signature
    * `msg` points to the `msglen` characters to be signed
    * `sig` holds the address at which to store the signature
    * If either `key` or `msg` is `NULL`, your function must fail
* `sig->sig` does not need to be zero-terminated. If you feel like you want to zero-terminate it, make sure that `sig->len` holds the size of the signature without the trailing zero byte
* Your function must return a pointer to the signature buffer upon success (`sig->sig`)
* `NULL` must be returned upon failure

File(s): [`ec_sign.c`](./ec_sign.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -I. -o ec_sign-test test/ec_sign-main.c provided/_print_hex_buffer.c ec_sign.c ec_create.c -lssl -lcrypto`

### :white_check_mark: 7. Signature verification
Write a function that verifies the signature of a given set of bytes, using a given EC_KEY **public key**

* Prototype: `int ec_verify(EC_KEY const *key, uint8_t const *msg, size_t msglen, sig_t const *sig);`, where:
    * `key` points to the `EC_KEY` structure containing the public key to be used to verify the signature
    * `msg` points to the `msglen` characters to verify the signature of
    * `sig` points to the signature to be checked
    * If either `key`, `msg` or `sig` is `NULL` your function must fail
* Your function must return 1 if the signature is valid, and 0 otherwise

File(s): [`ec_verify.c`](./ec_verify.c)\
Compiled: `gcc -Wall -Wextra -Werror -pedantic -I. -o ec_verify-test test/ec_verify-main.c provided/_print_hex_buffer.c ec_verify.c ec_sign.c ec_create.c -lssl -lcrypto`

### :white_check_mark: 8. Library
Write a Makefile that compiles all the previous functions and archives them into a static library for future use.

The library must be called `libhblk_crypto.a`, and your Makefile must define a rule for this file.

File(s): [`Makefile`](./Makefile)\
Compiled: `make libhblk_crypto.a`

---

## Student
* **Samuel Pomeroy** - [allelomorph](github.com/allelomorph)
