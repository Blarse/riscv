#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define DEBUG

#include "common.c"

#include "lex.h"
#include "lex.c"

#include "tables.h"

static uint32 line_number = 1;

void print_token(Token *token)
{
	printf("%d: Token <%s> ", line_number, tok_to_str[token->type]);

	if(token->type == TOK_LITERAL)
	{
		printf("[val = %u (%d)] [type = %s] [text = '%s']", token->value,
			   token->value, lit_to_str[token->literal_type], token->name);
	}
	else if(token->type == TOK_ERROR)
	{
		printf("[error = %s '%c']", err_to_str[token->error_code], token->value);
	}
	else if(token->type >= TOK_LABLE)
	{
		printf("[text = '%s']", token->name);
	}

	putchar('\n');
}

void pass1(FILE *input_file)
{
	uint32 location_counter;
	Token current_token;

#if defined(DEBUG)
	current_token.type = TOK_EOL;
#endif

	do
	{

#if defined(DEBUG)
		if(current_token.type == TOK_EOL)
		{
			fpos_t pos;
			size_t len = 0;
			char *debug_line = NULL;

			fgetpos(input_file, &pos);
			len = getline(&debug_line, &len, input_file);
			debug_line[len-1] = '\0';
			fsetpos(input_file, &pos);
			printf("'%s':\n", debug_line);
			free(debug_line);
		}
#endif

		current_token.type = 0;
		current_token.literal_type = 0;
		current_token.value = 0;
		current_token.name = NULL;
		next_token(&current_token, input_file);

		print_token(&current_token);

		if(current_token.type == TOK_EOL)
		{
			printf("\n");
			line_number++;
		}



	} while(current_token.type != TOK_EOF);

}

void lex_test(FILE *input_file)
{
	printf("Test\n");
	printf(__func__);
	Token token = {};
	read_hex(&token, input_file);
	printf("a= %d", token.value);

}

int main(int argc, char **argv)
{


	//TODO: Add listing file
	if(argc != 3)
		panic("Wrong arguments\nUsage: %s <input-file> <output-file>", argv[0]);

	char *intput_file_name = argv[1];
	char *output_file_name = argv[2];

	FILE *input_file = fopen(intput_file_name, "r");
	if(!input_file)
		panic("Can't open '%s' for reading", intput_file_name);

	FILE *output_file = fopen(output_file_name, "w");
	if(!output_file)
		panic("Can't create '%s'", output_file_name);


	/* lex_test(input_file); */
	/* exit(0); */

	pass1(input_file);

	return 0;
}
