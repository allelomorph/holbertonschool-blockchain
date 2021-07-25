/* assumes openssl and libssl-dev versions "1.0.1f-1ubuntu2.27 amd64" */
/* OPENSSL_VERSION_NUMBER: 0x01000106F */
/* SSLEAY_VERSION: 'OpenSSL 1.0.1f 6 Jan 2014' */


#include "hblk_crypto.h"
/* ERR_free_strings */
/* #include <openssl/err.h> */
/* EVP_cleanup */
/* #include <openssl/evp.h> */
/* CRYPTO_cleanup_all_ex_data */
#include <openssl/crypto.h>


/**
 * OpenSSLGlobalCleanup - runs "brutal" (thread unsafe) application-global
 *   cleanup functions to free any internal tables allocated by OpenSSL
 *
 * Note: It has been observed with valgrind that the following function
 *   subroutine calls:
 *     - PEM_write* -> BIO_new -> BIO_set -> ...
 *     - PEM_read* -> PEM_ASN1_read -> BIO_new -> ...
 *     - ECDSA_sign -> ECDSA_sign_ex -> ECDSA_do_sign_ex -> ...
 *   all end with -> CRYPTO_malloc -> malloc, allocating data that persists
 *   until the process ends with a seeming leak of `still reachable: 416
 *   bytes in 6 blocks`.
 *
 *   At `https://www.openssl.org/docs/faq.html#PROG14`, "I think I've
 *   detected a memory leak...", this is described as "an OpenSSL internal
 *   table that is allocated when an application starts up". The three
 *   functions below are listed there as application-global, non-thread safe
 *   options for freeing these tables.
 *
 *   See also `https://wiki.openssl.org/index.php/Library_Initialization` and
 *   `https://stackoverflow.com/a/29927669` regarding best practices
 *   for OpenSSL initialization/uninitialization.
 */
void OpenSSLGlobalCleanup(void)
{
	/* after calls to ERR_load_crypto_strings, SSL_load_error_strings: */
	/* ERR_free_strings(); */

	/* after calls to OpenSSL_add_all_digests/_algorithms/_ciphers: */
	/* EVP_cleanup(); */

	/*
	 * no 1.1.1 man page, but see above links plus:
	 * `https://www.openssl.org/docs/man1.1.1/man3/CRYPTO_EX_free.html`
	 */
	/* after calls to CRYPTO_malloc: */
	CRYPTO_cleanup_all_ex_data();
}
