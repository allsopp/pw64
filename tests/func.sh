#!/bin/sh

PW64="$1"

echo "1..9"

(: | $PW64 foo 2>/dev/null) || echo "ok 1 - null input"

(echo | $PW64 -n 1 foo >/dev/null 2>&1) && echo "ok 2 - empty input"

nl="$(echo | $PW64 -n 1 foo 2>/dev/null | wc -l)"
[ "$nl" -eq 1 ] && echo "ok 3 - newline"

nl="$(echo | $PW64 -n 1 -c foo 2>/dev/null | wc -l)"
[ "$nl" -eq 0 ] && echo "ok 4 - newline chomped"

(printf "bar\nbar\n" | $PW64 -2 -n1 foo >/dev/null 2>&1) \
	&& echo "ok 5 - match"

(printf "bar\nbaz\n" | $PW64 -2 -n1 foo >/dev/null 2>&1) \
	|| echo "ok 6 - non-match"

EXPECT="$(mktemp)" || exit 1
ACTUAL="$(mktemp)" || exit 1

perl -MPBKDF2::Tiny -e 'print PBKDF2::Tiny::derive("SHA-256", "foo:bar", "", 1)' \
	| openssl base64 \
	| cut -c1-16 > "$EXPECT"
echo bar | $PW64 -l16 -n1 foo > "$ACTUAL"
cmp "$EXPECT" "$ACTUAL" && echo "ok 7 - pbkdf2 (1 iterations)"

perl -MPBKDF2::Tiny -e 'print PBKDF2::Tiny::derive("SHA-256", "foo:bar", "", 100)' \
	| openssl base64 \
	| cut -c1-16 > "$EXPECT"
echo bar | $PW64 -l16 -n100 foo > "$ACTUAL"
cmp "$EXPECT" "$ACTUAL" && echo "ok 8 - pbkdf2 (100 iterations)"

perl -MPBKDF2::Tiny -e 'print PBKDF2::Tiny::derive("SHA-256", "foo:bar", "", 10000)' \
	| openssl base64 \
	| cut -c1-16 > "$EXPECT"
echo bar | $PW64 -l16 -n10000 foo > "$ACTUAL"
cmp "$EXPECT" "$ACTUAL" && echo "ok 9 - pbkdf2 (10000 iterations)"
