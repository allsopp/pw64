#include <stdlib.h>
#include <stdbool.h>
void password_round(struct password *, unsigned char *, size_t);
char * password_prompt(struct password *, const char *);
void password_print(struct password *, size_t);
size_t password_encode(struct password *);
unsigned char *password_derive(struct password *, unsigned long);
void password_echo(bool);
int password_init(struct password *, const char *);
