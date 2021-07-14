/* size_t */
#include <stddef.h>
/* uint8_t */
#include <stdint.h>
/* malloc free */
#include <stdlib.h>
/* fprintf sprintf printf */
#include <stdio.h>


/*
 * Note: File _print_hex_buffer.c mentioned in (443) 0x00. Blockchain - Crypto
 * as a provided file, but could not be found in project outline, so this
 * version was written by the student, inferring output from examples.
*/

/**
 * _print_hex_buffer - prints an array of uint8_t values in 2-digit lowercase
 *   hex format
 *
 * @hex_buf: buffer of values to print
 * @hex_buf_sz: amount of bytes in buffer
 */
void _print_hex_buffer(uint8_t *hex_buf, size_t hex_buf_sz)
{
	size_t i, j, print_buf_sz;
	char *print_buf;

	print_buf_sz = (hex_buf_sz * 2) + 1;
	print_buf = malloc(sizeof(char) * print_buf_sz);
	if (!print_buf)
	{
		fprintf(stderr, "_print_hex_buffer: malloc failure\n");
		return;
	}

	for (i = 0, j = 0; i < hex_buf_sz && j < print_buf_sz; i++, j += 2)
		sprintf(print_buf + j, "%02x", hex_buf[i]);
	print_buf[print_buf_sz - 1] = '\0';

	printf("%s", print_buf);
	free(print_buf);
}
