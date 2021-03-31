#include <stdlib.h>
#include <stdbool.h>
void password_round(struct password *, unsigned char *, size_t);
void password_delete(struct password *);
void password_prompt(struct password *, char const *);
void password_print(struct password *, size_t);
size_t password_encode(struct password *);
unsigned char *password_derive(struct password *, unsigned long);
void password_echo(bool);
struct password *password_new(char const *);
