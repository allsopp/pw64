#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LENGTH( x ) (4*((2+x)/3))

/*
 * The input is split into 6-bit chunks. Each chunk corresponds to a Base64
 * character from the set "A-Za-z0-9+/". The mapping is implemented below:
 */
static char
base64char(unsigned char b)
{
	if (b == 63) return '/';
	if (b == 62) return '+';

	if (b >= 52)			/* digits */
		return b - 52 + '0';

	if (b >= 26)			/* lowercase */
		return b - 26 + 'a';

	return b + 'A';			/* uppercase */
}

/*
 * In Base64, each character represents 6 bits of data, so every 4 Base64
 * characters represent 24 bits (3 bytes) of data, i.e.:
 *
 *   byte 1     byte 2     byte 3
 * [AAAAAABB] [BBBBCCCC] [CCDDDDDD]
 *
 * The 'base64enc' function extracts the 6-bit chunks (A,B,C,D) from each
 * consecutive triplet of bytes, giving:
 *
 * - AAAAAA chunk 1
 * - BBBBBB chunk 2
 * - CCCCCC chunk 3
 * - DDDDDD chunk 4
 *
 * The result is a string where each 'chunk' is represented by the
 * corresponding 6-bit Base64 character (implemented in 'base64char').
 *
 * The string is then padded with '=' until its length is a multiple of 4.
 */
int
base64enc(char **output, const unsigned char *data, int bytes)
{
	int i, j = -1;
	char *buffer;

	assert(data);
	assert(output);

	buffer = malloc(1+ LENGTH(bytes));
	if (!buffer) {
		fputs("out of memory\n", stderr);
		return -1;
	}

	for (i = 0; i < bytes; i++) {
		switch (i%3) {
		case 0:
			buffer[++j] = data[i] >> 2;
			buffer[++j] = (data[i] & 0x03) << 4; /* 0b00000011 */
			break;
		case 1:
			buffer[j] |= data[i] >> 4;
			buffer[++j] = (data[i] & 0x0f) << 2; /* 0b00001111 */
			break;
		case 2:
			buffer[j] |= data[i] >> 6;
			buffer[++j] = (data[i] & 0x3f) << 0; /* 0b00111111 */
			break;
		}
	}

	/*
	 * At this point, 'j' is the index of last character Base64 string
	 * (before padding), so overwrite each byte with its Base64
	 * representation (in ASCII):
	 */
	assert(j < LENGTH(bytes));
	for (i = 0; i <= j; i++)
		buffer[i] = base64char(buffer[i]);

	/*
	 * The Base64 string must be padded with '=' characters until total
	 * length is an integer multiple of 4:
	 */
	while (i % 4)
		buffer[i++] = '=';
	buffer[LENGTH(bytes)] = '\0';

	*output = buffer;
	assert(strlen(*output) == (size_t)LENGTH(bytes));
	return strlen(*output);
}
