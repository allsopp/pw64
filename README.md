# pw64

[![builds.sr.ht status](https://builds.sr.ht/~allsopp/pw64.svg)](https://builds.sr.ht/~allsopp/pw64?)

Deterministic password manager with Base64 output

## Description

The command-line argument is combined with the primary password entered
interactively, and this is passed into `HMAC-SHA256` to generate a
deterministic password, encoded in Base64.

## Prerequisites

On Debian-based systems:

	# apt install gcc meson nettle-dev ninja-build pkg-config

## How to build

	$ meson setup build
	$ ninja -C build
	# ninja install -C build
