#include "types.h"
#include "base64.h"
#include "config.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <nettle/hmac.h>
#include <nettle/pbkdf2.h>

int
password_init(struct password *pw, const char *prefix)
{
	assert(pw);

	pw->input = malloc(1+ strlen(prefix) + strlen(DELIM) + MAXINPUT);
	if (!pw->input)
		return -1;

	pw->buffer = calloc(1, SHA256_DIGEST_SIZE);
	if (!pw->buffer)
		return -1;

	strcpy(pw->input, prefix);
	strcat(pw->input, DELIM);
	pw->primary = pw->input + strlen(pw->input);
	pw->prompt_count = 0;
	pw->output = NULL;

	return 0;
}

/* Disabling the ECHO bit prevents stdin being echoed: password is secret! */
void
password_echo(bool echo)
{
	struct termios t;

	tcgetattr(fileno(stdin), &t);
	if (echo)
		t.c_lflag |= ECHO;
	else
		t.c_lflag &= ~ECHO;
	tcsetattr(fileno(stdin), TCSADRAIN, &t);
}

char *
password_prompt(struct password *pw, const char *msg)
{
	char entry[MAXINPUT+1];
	assert(pw);

	if (isatty(fileno(stdin))) {
		fputs(msg, stderr);
		password_echo(false);
	}

	if (!(fgets(entry, sizeof entry, stdin))) {
		password_echo(true);
		fputs("couldn't read password\n", stderr);
		return NULL;
	}

	if (isatty(fileno(stdin))) {
		password_echo(true);
		fputs("\n", stderr);
	}

	/* fgets() reads until first newline, so any newline will be trailing */
	entry[strcspn(entry, "\r\n")] = '\0';

	if (pw->prompt_count++ /* short circuit on the first prompt */
		&& strlen(pw->primary) + strlen(entry) /* special case for empty string */
		&& strcmp(pw->primary, entry) != 0)
	{
		fputs("passwords do not match\n", stderr);
		return NULL;
	}

	return strcpy(pw->primary, entry);
}

size_t
password_encode(struct password *pw)
{
	assert(pw);
	free(pw->output);
	return base64enc(&pw->output, pw->buffer, SHA256_DIGEST_SIZE);
}

void
password_print(struct password *pw, size_t len)
{
	assert(pw);
	if (!pw->output)
		return;
	if (len < strlen(pw->output)) {
		char const *c = pw->output;
		while (len--)
			putchar(*(c++));
	} else {
		fputs(pw->output, stdout);
	}
}

unsigned char *
password_derive(struct password *pw, unsigned long rounds)
{
	struct hmac_sha256_ctx ctx;
	assert(pw);

	hmac_sha256_set_key(&ctx, strlen(pw->input),
		(const unsigned char *)pw->input);
	PBKDF2(&ctx,
		hmac_sha256_update,
		hmac_sha256_digest,
		SHA256_DIGEST_SIZE,
		rounds,
		0,
		NULL,
		SHA256_DIGEST_SIZE,
		pw->buffer);

	return pw->buffer;
}
