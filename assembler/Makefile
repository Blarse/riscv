.PHONY: all debug run clean

CC=gcc
CFLAGS=-ggdb -Wall

all: main

main: main.c
	$CC -c -o $@ $< $(CFLAGS)

debug:

run: main

clean:
