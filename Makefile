
CC=gcc

.PHONY: int com all

all: int com

int: src/bfi.c
	gcc src/bfi.c -o bfi

com: src/bfvm.c
	gcc src/bfvm.c -o bfvm

clean:
	rm -rf bfi bfvm obj

