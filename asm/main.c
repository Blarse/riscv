#include <stdio.h>
#include <string.h>
#include <ctype.h>


#include "common.c"

#include "lex.h"
#include "lex.c"

#include "tables.h"

static uint32 LineNumber = 1;

void print_token(token *Token)
{
	printf("%d: Token <%s> ", LineNumber, TokToStr[Token->Type]);
	if(Token->Type == TOK_LITERAL)
		printf("[val = %u (%d)] [type = %s]", Token->NumValue,
			   Token->NumValue, LitToStr[Token->LiteralType]);
	if(Token->Type == TOK_ERROR)
		printf("[error = %s '%c']", ErrToStr[Token->ErrorCode], Token->NumValue);
	putchar('\n');
}

void pass1(FILE *InputFile)
{
	uint32 LocationCounter;
	token CurrentToken;

	do
	{
		CurrentToken.Type = 0;
		CurrentToken.LiteralType = 0;
		CurrentToken.NumValue = 0;
		CurrentToken.Name = NULL;
		next_token(&CurrentToken, InputFile);

		print_token(&CurrentToken);

		if(CurrentToken.Type == TOK_EOL)
			LineNumber++;

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
