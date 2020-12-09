.PHONY: all debug run clean


CC=gcc
CFLAGS=-ggdb -Wall -O0

all: asm

asm: main.c lex.c lex.h tables.h common.c debug.c
	$(CC) -o $@ $< $(CFLAGS)



debug:

run: asm
	./asm samples/test.s test.o

rund: asm
	gdb --args asm samples/test.s test.o

clean:
	rm asm
