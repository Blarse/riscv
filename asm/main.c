#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define DEBUG
#define PRINT_TOKENS 0
#define PRINT_INPUT_LINES 0

#include "common.c"

#include "tables.h"
#include "instructions.h"
#include "lex.h"
#include "hashmap.h"

static uint32 line_number = 1;
static char *input_file_name = NULL;
static char *output_file_name = NULL;
static HM_Map *global_symbol_table = NULL;

#include "debug.c"

#include "lex.c"
#include "hashmap.c"
#include "parser.c"

#define EXPECT_COMMA()											\
	do{buf_free(current_token.name);							\
		next_token(&current_token, input_file);					\
		if(current_token.type != TOK_COMMA)						\
			panic("Parser error: .equ expected ','");			\
	}while(0)													\


void pass1(FILE *input_file)
{
	d_printf("Pass 1:\n");
	uint32 location_counter = 0;
	Token current_token = { .type = TOK_EOL};

	do
	{

		if(current_token.type == TOK_EOL)
		{
			print_input_line(location_counter, input_file);
		}

		next_token(&current_token, input_file);
		print_token(&current_token);

		//TODO(maybe): use string interning for fast comparison
		switch(current_token.type)
		{
		case TOK_DIRECTIVE:

			switch(parse_directive(current_token.name))
			{
			case DIR_ALIGN:
				buf_free(current_token.name);
				next_token(&current_token, input_file);
				if(current_token.type == TOK_LITERAL)
				{
					switch(current_token.value)
					{
					case 2:
						location_counter = ALIGN2(location_counter);
						break;
					case 4:
						location_counter = ALIGN4(location_counter);
						break;
					case 8:
						location_counter = ALIGN8(location_counter);
						break;
					default:
						panic("Wrong .align argument");
					}
				}
				else
				{
					panic("Wrong .align argument");
				}
				break;


			case DIR_ORG:
				buf_free(current_token.name);
				next_token(&current_token, input_file);
				if(current_token.type == TOK_LITERAL)
				{
					location_counter = current_token.value;
				}
				else
				{
					panic("Wrong .org argument");
				}
				break;

			case DIR_STRING:
			case DIR_ASCIZ:
			case DIR_ASCII://TODO: fix zero term string
				buf_free(current_token.name);
				next_token(&current_token, input_file);
				if(current_token.type == TOK_LITERAL &&
				   current_token.literal_type == LIT_STR)
				{
					location_counter += current_token.value;
				}
				else
				{
					panic("Wrong .string/.asciz argument");
				}
				break;

			case DIR_BYTE:
				location_counter++;
				break;

			case DIR_2BYTE:
			case DIR_HALF:
			case DIR_SHORT:
				location_counter += 2;
				break;

			case DIR_4BYTE:
			case DIR_WORD:
			case DIR_QUAD:
				location_counter += 4;
				break;

			case DIR_ZERO:
			case DIR_SPACE:
				buf_free(current_token.name);
				next_token(&current_token, input_file);
				if(current_token.type == TOK_LITERAL)
				{
					location_counter += current_token.value;
				}
				break;

			case DIR_EQU:
			{
				buf_free(current_token.name);
				next_token(&current_token, input_file);
				if(current_token.type != TOK_IDENTIFIER)
					panic("Parser error: .equ expected identifier");
				char* key = current_token.name;

				EXPECT_COMMA();

				buf_free(current_token.name);
				next_token(&current_token, input_file);
				if(current_token.type != TOK_LITERAL)
					panic("Parser error: .equ expected literal");

				hm_insert(global_symbol_table, key, current_token.value);
				buf_free(key);



			} break;

			case DIR_UNDEFINED:
			default:
				//panic("Parser error: undefined directive");
				break;
			}

			while(current_token.type != TOK_EOL)
			{
				buf_free(current_token.name);
				next_token(&current_token, input_file);
				print_token(&current_token);
			}
			line_number++;

			break;

		case TOK_IDENTIFIER:
			switch(parse_instruction(current_token.name))
			{
			case INST_UNDEFINED:
				panic("Parser error: undefined instruction '%s'", current_token.name);
				break;
			default:
				location_counter += 4;
				break;
			}

			while(current_token.type != TOK_EOL)
			{
				buf_free(current_token.name);
				next_token(&current_token, input_file);
				print_token(&current_token);
			}
			line_number++;

			break;

		case TOK_LABEL:
			//d_printf("LABLE: %-10s = %d(%#x)\n",
			//		current_token.name, location_counter, location_counter);
			current_token.name[buf_len(current_token.name)-2] = '\0';
			hm_insert(global_symbol_table, current_token.name, location_counter);

			break;

		case TOK_EOL:
			line_number+=current_token.value+1;
			break;

		default:
			break;
		}

		buf_free(current_token.name);
	} while(current_token.type != TOK_EOF);

}

void pass2(FILE *input_file, FILE *output_file)
{
	d_printf("Pass 2:\n");
	uint32 location_counter = 0;
	Token current_token = { .type = TOK_EOL};

	do
	{

		if(current_token.type == TOK_EOL)
		{
			print_input_line(location_counter, input_file);
		}

		next_token(&current_token, input_file);
		print_token(&current_token);

		//TODO(maybe): use string interning for fast comparison
		switch(current_token.type)
		{
		case TOK_DIRECTIVE:
			while(current_token.type != TOK_EOL)
			{
				buf_free(current_token.name);
				next_token(&current_token, input_file);
				print_token(&current_token);
			}
			break;

		case TOK_IDENTIFIER:
		{
			enum Instructions inst = parse_instruction(current_token.name);
			if(inst == INST_UNDEFINED)
				panic("Parser error: undefined instruction '%s'", current_token.name);

			uint32 rd;
			if(i2[inst].format & IN_RD)
			{
				buf_free(current_token.name);
				next_token(&current_token, input_file);
				rd = parse_register(current_token.name);
				if(rd == REG_UNDEFINED)
					panic("Parser error: undefined register '%s'", current_token.name);
			}



			while(current_token.type != TOK_EOL)
			{
				buf_free(current_token.name);
				next_token(&current_token, input_file);
				print_token(&current_token);
			}
		} break;

		case TOK_LABEL:
			while(current_token.type != TOK_EOL)
			{
				buf_free(current_token.name);
				next_token(&current_token, input_file);
				print_token(&current_token);
			}

			break;

		case TOK_EOL:
		case TOK_EOF:
			break;
		default:
			panic("Parser error: unexpected token '%s' <%s>",
				  current_token.name, tok_to_str[current_token.type]);
		}

		buf_free(current_token.name);
	} while(current_token.type != TOK_EOF);


}

void parser_test()
{
	assert(parse_directive(".align") == DIR_ALIGN);
	assert(parse_directive(".text") == DIR_TEXT);
	assert(parse_directive(".quad") == DIR_QUAD);
	assert(parse_directive("something") == DIR_UNDEFINED);
	assert(parse_directive(".") == DIR_LC);
	assert(parse_directive("") == DIR_UNDEFINED);
}

void align_test()
{
	assert(ALIGN2(0) == 0);
	assert(ALIGN2(1) == 2);
	assert(ALIGN2(2) == 2);
	assert(ALIGN2(3) == 4);
	assert(ALIGN2(4) == 4);
	assert(ALIGN2(5) == 6);
	assert(ALIGN2(0x12345) == 0x12346);

	assert(ALIGN4(0) == 0);
	assert(ALIGN4(1) == 4);
	assert(ALIGN4(2) == 4);
	assert(ALIGN4(3) == 4);
	assert(ALIGN4(4) == 4);
	assert(ALIGN4(5) == 8);
	assert(ALIGN4(0x12345) == 0x12348);

	assert(ALIGN8(0) == 0);
	assert(ALIGN8(1) == 8);
	assert(ALIGN8(2) == 8);
	assert(ALIGN8(5) == 8);
	assert(ALIGN8(7) == 8);
	assert(ALIGN8(8) == 8);
	assert(ALIGN8(9) == 16);
	assert(ALIGN8(0x1234a) == 0x12350);
}

void hashmap_test()
{
	HM_Map *map = hm_create_map(9);
	hm_insert(map, "Key1", 1);
	hm_insert(map, "Key2", 2);
	hm_insert(map, "Key3", 3);
	hm_insert(map, "Key4", 4);
	hm_insert(map, "Key5abcdef", 5);
	hm_insert(map, "Key6", 6);
	hm_insert(map, "Key7", 7);
	hm_insert(map, "Key8", 8);
	hm_insert(map, "Key9", 9);
	hm_insert(map, "Key10", 10);
	hm_insert(map, "Key11", 11);
	assert(hm_search(map, "Key4") == 4);
	assert(hm_search(map, "Key5abcdef") == 5);
	assert(hm_delete(map, "Key5abcdef") == 5);
	assert(hm_search(map, "Key5abcdef") == 0);

	hm_print_map(map);
}

void run_tests()
{
	/* parser_test(); */
	/* align_test(); */
	hashmap_test();
	exit(0);
}

int main(int argc, char **argv)
{



	//TODO: Add listing file
	if(argc != 3)
		panic("Wrong arguments\nUsage: %s <input-file> <output-file>", argv[0]);

	input_file_name = argv[1];
	output_file_name = argv[2];

	FILE *input_file = fopen(input_file_name, "r");
	if(!input_file)
		panic("Can't open '%s' for reading", input_file_name);

	FILE *output_file = fopen(output_file_name, "w");
	if(!output_file)
		panic("Can't create '%s'", output_file_name);

	//run_tests();

	global_symbol_table = hm_create_map(409);
	pass1(input_file);
	/* hm_print_map(global_symbol_table); */

	rewind(input_file);
	pass2(input_file, output_file);

	return 0;
}
