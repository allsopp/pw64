#include "types.h"

#include "config.h"
#include "password.h"

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define USAGE "usage: %s [-2] [-c] [-l <num>] [-n <num>] <word>\n" \
              "       %s -v\n"

static void
reset(int sig)
{
	(void)sig;
	password_echo(true);
	fputc('\n', stderr);
	exit(EXIT_FAILURE);
}

static void
usage(const char **s)
{
	fprintf(stderr, USAGE, *s, *s);
}

int
main(int argc, char **argv)
{
	int c;
	unsigned long rounds = ROUNDS;
	size_t length = LENGTH;
	bool chomp = false;
	bool twice = false;
	const char *u[] = { *argv };
	struct password pw;

	while ((c = getopt(argc, argv, "2chl:n:v")) != -1) {
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
			fputs("number of iterations must be at least one\n", stderr);
			return EXIT_FAILURE;
		case 'l':
			if ((length = atoi(optarg)) > 0)
				break;
			fputs("minimum password length is one character\n", stderr);
			return EXIT_FAILURE;
		case 'v':
			printf("%s (version %s)\n", PACKAGE_NAME, PACKAGE_VERSION);
			return EXIT_SUCCESS;
		case 'h':
		default:
			usage(u);
			return EXIT_FAILURE;
		}
	}

	argc -= optind;
	argv += optind;

	if (argc < 1) {
		usage(u);
		return EXIT_FAILURE;
	}

	(void)signal(SIGINT, reset);

	if (password_init(&pw, *argv))
		return EXIT_FAILURE;

	for (int i = 0; i < 2; ++i) {
		const char *s = i ? "Repeat primary password: "
		                  : "Enter primary password: ";
		if (!password_prompt(&pw, s))
			return EXIT_FAILURE;
		if (!twice)
			break;
	}

	(void)password_derive(&pw, rounds);
	if (!password_encode(&pw))
		return EXIT_FAILURE;

	password_print(&pw, length);

	if (!chomp)
		putchar('\n');

	return EXIT_SUCCESS;
}
