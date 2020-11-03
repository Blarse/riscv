#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "common.c"
#include "lex.c"

#define LINE_BUFFER_SIZE 256

/*
TODO: Assembly-time errors
 */


typedef struct
{
	uint32 Value;
	char Name[32];
} symbol;



int isempty(const char *String)
{
	while(*String != '\0')
	{
		if(!isspace(*String))
			return 0;
		String++;
	}
	return 1;
}


void parse_line(char *Stream, char *Lable,
				char *Mnemonic,	char *Operands, char *Comment)
{

}

void pass1(FILE *InputFile)
{
	uint32 LocationCounter;
	token CurrentToken;


	do
	{
		next_token(&CurrentToken, InputFile);
	} while(CurrentToken.Type != TOK_EOF);

}

void lex_test(FILE *InputFile)
{
	printf("Test\n");
	printf(__func__);
	token Token = {};
	read_hex(&Token, InputFile);
	printf("a= %d", Token.NumValue);

}

int main(int ArgsCount, char **Args)
{

	//TODO: Add listing file
	if(ArgsCount != 3)
		panic("Wrong arguments\nUsage: %s <input-file> <output-file>", Args[0]);

	char *InputFileName = Args[1];
	char *OutputFileName = Args[2];

	FILE *InputFile = fopen(InputFileName, "r");
	if(!InputFile)
		panic("Can't open '%s' for reading", InputFileName);

	FILE *OutputFile = fopen(OutputFileName, "w");
	if(!OutputFile)
		panic("Can't create '%s'", OutputFileName);


	/* lex_test(InputFile); */
	/* exit(0); */

	pass1(InputFile);

	return 0;
}
