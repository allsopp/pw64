#define _POSIX_C_SOURCE 2

#include "types.h"

#include "config.h"
#include "password.h"

#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define USAGE "usage: %s [-2] [-c] [-l <num>] [-n <num>] <word>\n"

static void
reset(int sig)
{
	if (sig) {
		password_echo(true);
		fputc('\n', stderr);
		exit(EXIT_FAILURE);
	}
}

static void
help(char const *arg0)
{
	fprintf(stderr, USAGE, arg0);
}

int
main(int argc, char **argv)
{
	int c;
	unsigned long rounds = ROUNDS;
	size_t length = LENGTH;
	bool chomp = false;
	bool twice = false;
	char const *arg0 = *argv;
	struct password *pw;

	while ((c = getopt(argc, argv, "2chl:n:")) != -1) {
		switch(c) {
		case '2':
			twice = true;
			break;
		case 'c':
			chomp = true;
			break;
		case 'n':
			if ((rounds = atoi(optarg)) > 0)
				break;
			fputs("error: number of iterations"
				" must be at least one\n", stderr);
			return EXIT_FAILURE;
		case 'l':
			if ((length = atoi(optarg)) > 0)
				break;
			fputs("error: minimum password length"
				" is one character\n", stderr);
			return EXIT_FAILURE;
		case 'h':
		default:
			help(arg0);
			return EXIT_FAILURE;
		}
	}

	argc -= optind;
	argv += optind;

	if (argc < 1) {
		help(arg0);
		return EXIT_FAILURE;
	}

	if (signal(SIGINT, reset) == SIG_ERR)
		fputs("WARNING: couldn't install SIGINT handler\n", stderr);

	pw = password_new(*argv);

	password_prompt(pw, "Enter master password: ");
	if (twice)
		password_prompt(pw, "Verify master password: ");

	password_derive(pw, rounds);

	/* Digested input, encoded in Base64 */
	password_encode(pw);
	password_print(pw, length);

	password_delete(pw);

	/* Chomp suppresses trailing newline: */
	if (!chomp)
		putchar('\n');

	return EXIT_SUCCESS;
}
