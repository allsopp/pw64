#include <stdlib.h>
#include <assert.h>
#include "../base64.c"

void
base64char_test(void)
{
	const char * expected =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";

	for (int i = 0; i < 64; ++i)
		assert(expected[i] == base64char(i));
}

void
base64enc_test(void)
{
	struct __base64_string {
		char const * in;
		char const * out;
	};

	struct __base64_string s[] = {
		{ "", "" },
		{ "f", "Zg==" },
		{ "fo", "Zm8=" },
		{ "foo", "Zm9v" },
		{ "foo ", "Zm9vIA==" },
		{ "foo b", "Zm9vIGI=" },
		{ "foo ba", "Zm9vIGJh" },
		{ "foo bar", "Zm9vIGJhcg==" },
		{ "foo bar!", "Zm9vIGJhciE=" },
		{ "foo bar!\n", "Zm9vIGJhciEK" }
	};

	int count = sizeof s / sizeof(struct __base64_string);

	for (int i = 0; i < count; ++i) {
		char * buffer;
		base64enc(&buffer, (unsigned char const *)s[i].in, i);
		assert(!strcmp(buffer, s[i].out));
	}
}

int
main(void) {
	base64char_test();
	base64enc_test();
	return EXIT_SUCCESS;
}
