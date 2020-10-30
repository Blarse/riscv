.PHONY: all debug run clean


CC=gcc
CFLAGS=-ggdb -Wall -O0

all: main

main: main.c
	$(CC) -o $@ $< $(CFLAGS)

debug:

run: main
	./main samples/test.s test.o

clean:
	rm main
