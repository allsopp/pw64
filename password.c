#define _POSIX_C_SOURCE 200809L

#include "types.h"

#include "base64.h"
#include "config.h"

#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <nettle/hmac.h>
#include <nettle/pbkdf2.h>

struct password *
password_new(char const *prefix)
{
	struct password *pw;

	pw = malloc(sizeof *pw);
	if (pw == NULL) {
		fputs("ERROR: couldn't allocate memory\n", stderr);
		exit(EXIT_FAILURE);
	}
	pw->prompt_count = 0;

	pw->input = malloc(1+ strlen(prefix) + strlen(DELIM) + MAXINPUT);
	if (pw->input == NULL) {
		fputs("ERROR: couldn't allocate memory\n", stderr);
		exit(EXIT_FAILURE);
	}
	strcpy(pw->input, prefix);
	strcat(pw->input, DELIM);
	pw->master = pw->input + strlen(pw->input);

	pw->output = strdup(""); /* just a placeholder that can be freed later */
	if (pw->output == NULL) {
		fputs("ERROR: couldn't allocate memory\n", stderr);
		exit(EXIT_FAILURE);
	}

	pw->buffer = calloc(1, SHA256_DIGEST_SIZE);
	if (pw->buffer == NULL) {
		fputs("ERROR: couldn't allocate memory\n", stderr);
		exit(EXIT_FAILURE);
	}

	return pw;
}

void
password_delete(struct password *pw)
{
	free(pw->input);
	free(pw->buffer);
	free(pw->output);
	free(pw);
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
password_prompt(struct password *pw, char const *msg)
{
	char entry[MAXINPUT+1];

	if (isatty(fileno(stdin))) {
		fputs(msg, stderr);
		password_echo(false);
	}

	if ((fgets(entry, sizeof entry, stdin)) == NULL) {
		password_echo(true);
		fputs("ERROR: couldn't read password\n", stderr);
		exit(EXIT_FAILURE);
	}

	if (isatty(fileno(stdin))) {
		password_echo(true);
		fputs("\n", stderr);
	}

	/* fgets() reads until first newline, so any newline will be trailing */
	entry[strcspn(entry, "\r\n")] = '\0';

	if (pw->prompt_count++ /* short circuit on the first prompt */
		&& strlen(pw->master) + strlen(entry) /* special case for empty string */
		&& strcmp(pw->master, entry) != 0)
	{
		fputs("ERROR: passwords do not match\n", stderr);
		exit(EXIT_FAILURE);
	}

	return strcpy(pw->master, entry);
}

size_t
password_encode(struct password *pw)
{
	free(pw->output);
	return base64enc(&(pw->output), pw->buffer, SHA256_DIGEST_SIZE);
}

void
password_print(struct password *pw, size_t len)
{
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

	hmac_sha256_set_key(&ctx, strlen(pw->input),
		(unsigned char const *)pw->input);
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
