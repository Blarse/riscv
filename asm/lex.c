#include <limits.h>
#include "lex.h"

static void read_hex(Token *token, FILE *input_stream);
static void read_bin(Token *token, FILE *input_stream);
static void read_dec(Token *token, FILE *input_stream);
static void read_string(Token *token, FILE *input_stream);
static void read_char(Token *token, FILE *input_stream);
static void read_comment(FILE *input_stream);
static void read_multiline_comment(FILE *input_stream);

int8 char_to_int[256] =
{
	['1'] = 1,
	['2'] = 2,
	['3'] = 3,
	['4'] = 4,
	['5'] = 5,
	['6'] = 6,
	['7'] = 7,
	['8'] = 8,
	['9'] = 9,
	['A'] = 10, ['a'] = 10,
	['B'] = 11, ['b'] = 11,
	['C'] = 12, ['c'] = 12,
	['D'] = 13, ['d'] = 13,
	['E'] = 14, ['e'] = 14,
	['F'] = 15, ['f'] = 15,
};

#define CASE_ERROR(t,e,v)\
	do{t->type = TOK_ERROR; t->error_code = e; t->value = v;}while(0)

void next_token(Token *token, FILE *input_stream)
{
repeat:;

	char cursor = fgetc(input_stream);

	switch(cursor)
	{

	case '#':
		read_comment(input_stream);
		goto repeat; //NOTE: Next token is expected to be TOK_EOL

	case '/':
		cursor = fgetc(input_stream);
		switch(cursor)
		{
		case '/':
			read_comment(input_stream);
			break;
		case '*':
			read_multiline_comment(input_stream);
			//TODO(idea): count new lines for proper line count
			break;
		default:
			CASE_ERROR(token, ERR_UNEXPECTED_CHAR, cursor);
			return;
		}
		goto repeat; //NOTE: Read next token after comment

	case ' ': case '\t': case '\v':	case '\f': case '\r':
		goto repeat; //NOTE: Skip whitespace chars

	case '0':
		cursor = fgetc(input_stream);

		switch(cursor)
		{
		case 'x':
		case 'X':
			read_hex(token, input_stream);
			break;
		case 'b':
		case 'B':
			read_bin(token, input_stream);
			break;
		default:
			ungetc(cursor, input_stream);
			read_dec(token, input_stream);
		}

		break;

	case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		ungetc(cursor, input_stream);
		read_dec(token, input_stream);

		cursor = fgetc(input_stream);
		if(cursor == ':')
		{
			token->type = TOK_LABLE;
			token->name[buf_len(token->name)-1] = cursor;
			buf_push(token->name, '\0');
		}
		else if(cursor == 'f' || cursor == 'b')
		{
			token->type = TOK_IDENTIFIER;

			token->name[buf_len(token->name)-1] = cursor;
			buf_push(token->name, '\0');
		}
		else
		{
			ungetc(cursor, input_stream);
		}


		break;



	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
	case 'g': case 'h': case 'i': case 'j':	case 'k': case 'l':
	case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
	case 's': case 't': case 'u': case 'v': case 'w': case 'x':
	case 'y': case 'z':
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
	case 'G': case 'H': case 'I': case 'J':	case 'K': case 'L':
	case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
	case 'S': case 'T':	case 'U': case 'V': case 'W': case 'X':
	case 'Y': case 'Z': case '@': case '_': case '%': case '.':
		token->type = TOK_IDENTIFIER;

		char *result = NULL;
		buf_push(result, cursor);

		if(cursor == '.')
		{
			token->type = TOK_DIRECTIVE;
			if(isalpha(cursor = fgetc(input_stream)))
			{
				buf_push(result, cursor);
			}
		}

		if(cursor == '%')
		{
			token->type = TOK_RELOC_FUNC;
			if(isalpha(cursor = fgetc(input_stream)))
			{
				buf_push(result, cursor);
			}
		}

		while((cursor = fgetc(input_stream)) != EOF)
		{
			if(cursor == '_' || isalnum(cursor))
			{
				buf_push(result, cursor);
			}
			else
			{
				break;
			}
		}

		if(cursor == ':')
		{
			buf_push(result, cursor);
			cursor = fgetc(input_stream); //NOTE: to make following ungetc work
			token->type = TOK_LABLE;
		}

		ungetc(cursor, input_stream);
		token->name = result;

		break;

	case '\'':
		read_char(token, input_stream);
		break;
	case '"':
		read_string(token, input_stream);
		break;

	case ',':
		token->type = TOK_COMMA;
		break;

	case '+':
		token->type = TOK_PLUS;
		break;

	case '-':
		token->type = TOK_MINUS;
		break;

	case '(':
		token->type = TOK_OPEN_PAREN;
		break;

	case ')':
		token->type = TOK_CLOSE_PAREN;
		break;


	case '\n':
		token->type = TOK_EOL;
		break;

	case EOF:
		if(ferror(input_stream))
			CASE_ERROR(token, ERR_FILE_INPUT, cursor);
		else
			token->type = TOK_EOF;
		break;


	default:
		CASE_ERROR(token, ERR_UNEXPECTED_CHAR, cursor);
	}
}


static void read_comment(FILE *input_stream)
{
	int32 temp;
	while((temp = fgetc(input_stream)) != EOF)
	{
		if(temp == '\n')
		{
			ungetc(temp, input_stream);
			return;
		}
	}
}

static void read_multiline_comment(FILE *input_stream)
{
	int32 temp;
	while((temp = fgetc(input_stream)) != EOF)
	{
		if(temp == '*')
		{
			if((temp = fgetc(input_stream)) == '/')
			{
				return;
			}
		}
	}
}

static void read_hex(Token *token, FILE *input_stream)
{
	int32 temp;
	uint32 result = 0;
	uint32 result_next = 0;

	token->type = TOK_LITERAL;
	token->literal_type = LIT_HEX;

	char *orig_string = NULL;
	buf_push(orig_string, '0');
	buf_push(orig_string, 'x');

	while((temp = fgetc(input_stream)) != EOF)
	{
		if(isxdigit(temp))
		{
			buf_push(orig_string, (char)temp);

			result_next = (result << 4) + char_to_int[temp];

			if(result_next < result)
			{
				CASE_ERROR(token, ERR_INT_OVERFLOW, result);
			}

			result = result_next;
		}
		else
		{
			buf_push(orig_string, '\0');
			ungetc(temp, input_stream);
			token->value = result;
			token->name = orig_string;
			return;
		}
	}
}

static void read_bin(Token *token, FILE *input_stream)
{
	int32 temp;
	uint32 result = 0;
	uint32 result_next = 0;

	token->type = TOK_LITERAL;
	token->literal_type = LIT_BIN;

	char *orig_string = NULL;

	buf_push(orig_string, '0');
	buf_push(orig_string, 'b');


	while((temp = fgetc(input_stream)) != EOF)
	{
		if(temp == '0' || temp == '1')
		{
			buf_push(orig_string, (char)temp);

			result_next = (result << 1) + char_to_int[temp];

			if(result_next < result)
			{
				CASE_ERROR(token, ERR_INT_OVERFLOW, result);
			}

			result = result_next;
		}
		else
		{
			buf_push(orig_string, '\0');
			ungetc(temp, input_stream);
			token->value = result;
			token->name = orig_string;
			return;
		}
	}
}

static void read_dec(Token *token, FILE *input_stream)
{
	int32 temp;
	uint32 result = 0;
	uint32 result_next = 0;

	token->type = TOK_LITERAL;
	token->literal_type = LIT_DEC;

	char *orig_string = NULL;

	while((temp = fgetc(input_stream)) != EOF)
	{
		if(isdigit(temp))
		{
			buf_push(orig_string, (char)temp);

			result_next = (result * 10) + char_to_int[temp];

			if(result_next < result)
			{
				CASE_ERROR(token, ERR_INT_OVERFLOW, result);
			}

			result = result_next;
		}
		else
		{
			buf_push(orig_string, '\0');
			ungetc(temp, input_stream);
			token->value = result;
			token->name = orig_string;
			return;
		}
	}

}


static void read_string(Token *token, FILE *input_stream)
{
	int32 temp;
	char *result = NULL;

	token->type = TOK_LITERAL;
	token->literal_type = LIT_STR;

	while((temp = fgetc(input_stream)) != EOF)
	{

		if(temp == '"')
		{
			buf_push(result, '\0');
			token->name = result;
			token->value = buf_len(result);
			return;
		}
		else if(isprint(temp))
		{
			buf_push(result, temp);
		}
		else
		{
			CASE_ERROR(token, ERR_UNEXPECTED_CHAR, temp);
			return;
		}
	}
}

static void read_char(Token *token, FILE *input_stream)
{
	int32 temp;

	token->type = TOK_LITERAL;
	token->literal_type = LIT_CHR;

	char *orig_string = NULL;

	temp = fgetc(input_stream);
	if(temp == '\\')
	{
		buf_push(orig_string, '\\');
		temp = fgetc(input_stream);
		switch(temp)
		{
		case 'n':
			buf_push(orig_string, 'n');
			token->value = '\n';
			break;
			//TODO
		}
	}
	else if(isprint(temp))
	{
		buf_push(orig_string, (char)temp);
		token->value = temp;
	}

	buf_push(orig_string, '\0');
	token->name = orig_string;
	temp = fgetc(input_stream);
	if(temp != '\'')
	{
		CASE_ERROR(token, ERR_UNEXPECTED_CHAR, temp);
	}
	return;
}
