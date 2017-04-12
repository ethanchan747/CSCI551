/* 
 * aes-test.c
 * by John Heidemann
 *
 * inspired by code by Ben Miller
 * (Small sample for how to use Blowfish from OpenSSL
 *  http://www.eecis.udel.edu/~bmiller/cis364/2012s/hw/hw3/code/blowfish-test.c)
 *
 * Sadly, as of 2012-10-01 and openssl-1.0.0j
 * there are no manual pages for AES in openssl's libcrypto.
 * However, the header file /usr/include/openssl/aes.h
 * and the manual pages for blowfish(3) are a reasonable starting point.
 *
 * Compile in Linux (tested with Fedora-17) with:
 *	gcc -o $@ -g aes-test.c -lcrypto
 *
 */

/* uncomment next line to build a library by removing main(). */
/* #define IS_LIBRARY */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <openssl/aes.h>
#include <limits.h>
#include <assert.h>
#include "aes-test.h"

int main()
{
	unsigned char *key_text = "password1234568";  /* NOT a good password :-) */
	unsigned char key_data[AES_KEY_LENGTH_IN_CHARS];
	unsigned char *clear_text = "Four score and seven years ago our fathers brought forth on this continent a new nation, conceived in liberty, and dedicated to the proposition that all men are created eq";
	int clear_text_len = strlen(clear_text) + 1; /* add one for null termination */

	unsigned char *crypt_text;
	int crypt_text_len;
	unsigned char *clear_crypt_text;
	int clear_crypt_text_len;

	AES_KEY enc_key;
	AES_KEY dec_key;

	/*
	 * Fill in the 128-bit binary key with some text
	 * better would be to compute the sha1 has of the text,
	 * but this is OK for a class project.
	 */
	memset(key_data, 0, sizeof(key_text));
	strncpy(key_data, key_text, MIN(strlen(key_text), sizeof(key_data)));
	/* Now key_data is the 128-bit binary value that AES will use as a key. */

	/* test out encryption */
	class_AES_set_encrypt_key(key_data, &enc_key);
	class_AES_encrypt_with_padding(clear_text, clear_text_len, &crypt_text, &crypt_text_len, &enc_key);
	printf("%s\n", crypt_text);

	class_AES_set_decrypt_key(key_data, &enc_key);
	class_AES_decrypt_with_padding(crypt_text, crypt_text_len, &clear_crypt_text, &clear_crypt_text_len, &enc_key);
	printf("%s\n", clear_crypt_text);

	/* caller must free the buffers */
	free(crypt_text);
	free(clear_crypt_text);

	exit(0);
}

