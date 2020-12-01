#include <limits.h>
#include "lex.h"

static void read_hex(token *Token, FILE *InputStream);
static void read_bin(token *Token, FILE *InputStream);
static void read_dec(token *Token, FILE *InputStream);
static void read_string(token *Token, FILE *InputStream);
static void read_char(token *Token, FILE *InputStream);
static void read_comment(FILE *InputStream);
static void read_multiline_comment(FILE *InputStream);

int8 CharToInt[] =
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

#define CASE_ERROR(t,e,v) do{t->Type = TOK_ERROR; t->ErrorCode = e; t->Value = v;}while(0)

void next_token(token *Token, FILE *InputStream)
{

	Token->Type = TOK_UNDEFINED;
	Token->Value = 0;
repeat:;

	char Cursor = fgetc(InputStream);

	switch(Cursor)
	{
	case '#':
		read_comment(InputStream);
		goto repeat;
	case '/':
		Cursor = fgetc(InputStream);
		switch(Cursor)
		{
		case '/':
			read_comment(InputStream);
			break;
		case '*':
			read_multiline_comment(InputStream);
			break;
		default:
			CASE_ERROR(Token, ERR_UNEXPECTED_CHAR, Cursor);
			return;
		}
	case ' ': case '\t': case '\v':	case '\f': case '\r':
		goto repeat;


	case '-':
		//TODO: negative numbers
		CASE_ERROR(Token, ERR_UNEXPECTED_CHAR, Cursor);

	case '0':
		Cursor = fgetc(InputStream);

		switch(Cursor)
		{
		case 'x':
		case 'X':
			read_hex(Token, InputStream);
			break;
		case 'b':
		case 'B':
			read_bin(Token, InputStream);
			break;
		default:
			ungetc(Cursor, InputStream);
			read_dec(Token, InputStream);
		}

		break;

	case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		ungetc(Cursor, InputStream);
		read_dec(Token, InputStream);
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
	case 'Y': case 'Z': case '_':
		break;

	case '.':
		Token->Type = TOK_DIRECTIVE;
		while((Cursor = fgetc(InputStream)) != EOF)
		{

		}
		break;

	case '\'':
		read_char(Token, InputStream);
		break;
	case '"':
		read_string(Token, InputStream);
		break;

	case ',':
		Token->Type = TOK_COMMA;
		break;

	case '\n':
		Token->Type = TOK_EOL;
		break;

	case EOF:
		if(ferror(InputStream))
			CASE_ERROR(Token, ERR_FILE_INPUT, Cursor);
		else
			Token->Type = TOK_EOF;
		break;
	default:
		CASE_ERROR(Token, ERR_UNEXPECTED_CHAR, Cursor);
	}
}


static void read_comment(FILE *InputStream)
{
	int32 Temp;
	while((Temp = fgetc(InputStream)) != EOF)
	{
		if(Temp == '\n')
		{
			ungetc(Temp, InputStream);
			return;
		}
	}
}

static void read_multiline_comment(FILE *InputStream)
{
	int32 Temp;
	while((Temp = fgetc(InputStream)) != EOF)
	{
		if(Temp == '*')
		{
			if((Temp = fgetc(InputStream)) == '/')
			{
				return;
			}
		}
	}
}

static void read_hex(token *Token, FILE *InputStream)
{
	int32 Temp;
	uint32 Result = 0;
	uint32 ResultNext = 0;

	Token->Type = TOK_LITERAL;
	Token->LiteralType = LIT_HEX;

	char *OrigString = NULL;
	buf_push(OrigString, '0');
	buf_push(OrigString, 'x');

	while((Temp = fgetc(InputStream)) != EOF)
	{
		if(isxdigit(Temp))
		{
			buf_push(OrigString, (char)Temp);

			ResultNext = (Result << 4) + CharToInt[Temp];

			if(ResultNext < Result)
			{
				CASE_ERROR(Token, ERR_INT_OVERFLOW, Result);
			}

			Result = ResultNext;
		}
		else
		{
			buf_push(OrigString, '\0');
			ungetc(Temp, InputStream);
			Token->Value = Result;
			Token->Name = OrigString;
			return;
		}
	}
}

static void read_bin(token *Token, FILE *InputStream)
{
	int32 Temp;
	uint32 Result = 0;
	uint32 ResultNext = 0;

	Token->Type = TOK_LITERAL;
	Token->LiteralType = LIT_BIN;

	char *OrigString = NULL;

	buf_push(OrigString, '0');
	buf_push(OrigString, 'b');


	while((Temp = fgetc(InputStream)) != EOF)
	{
		if(Temp == '0' || Temp == '1')
		{
			buf_push(OrigString, (char)Temp);

			ResultNext = (Result << 1) + CharToInt[Temp];

			if(ResultNext < Result)
			{
				CASE_ERROR(Token, ERR_INT_OVERFLOW, Result);
			}

			Result = ResultNext;
		}
		else
		{
			buf_push(OrigString, '\0');
			ungetc(Temp, InputStream);
			Token->Value = Result;
			Token->Name = OrigString;
			return;
		}
	}
}

static void read_dec(token *Token, FILE *InputStream)
{
	int32 Temp;
	uint32 Result = 0;
	uint32 ResultNext = 0;

	Token->Type = TOK_LITERAL;
	Token->LiteralType = LIT_DEC;

	char *OrigString = NULL;

	while((Temp = fgetc(InputStream)) != EOF)
	{
		if(isdigit(Temp))
		{
			buf_push(OrigString, (char)Temp);

			ResultNext = (Result * 10) + CharToInt[Temp];

			if(ResultNext < Result)
			{
				CASE_ERROR(Token, ERR_INT_OVERFLOW, Result);
			}

			Result = ResultNext;
		}
		else
		{
			buf_push(OrigString, '\0');
			ungetc(Temp, InputStream);
			Token->Value = Result;
			Token->Name = OrigString;
			return;
		}
	}

}


static void read_string(token *Token, FILE *InputStream)
{
	int32 Temp;
	char *Result = NULL;

	Token->Type = TOK_LITERAL;
	Token->LiteralType = LIT_STR;

	while((Temp = fgetc(InputStream)) != EOF)
	{

		if(Temp == '"')
		{
			buf_push(Result, '\0');
			Token->Name = Result;
			return;
		}
		else if(isprint(Temp))
		{
			buf_push(Result, Temp);
		}
		else
		{
			CASE_ERROR(Token, ERR_UNEXPECTED_CHAR, Temp);
			return;
		}
	}
}

static void read_char(token *Token, FILE *InputStream)
{
	int32 Temp;

	Token->Type = TOK_LITERAL;
	Token->LiteralType = LIT_CHR;

	char *OrigString = NULL;

	Temp = fgetc(InputStream);
	if(Temp == '\\')
	{
		buf_push(OrigString, '\\');
		Temp = fgetc(InputStream);
		switch(Temp)
		{
		case 'n':
			buf_push(OrigString, 'n');
			Token->Value = '\n';
			break;
			//TODO
		}
	}
	else if(isprint(Temp))
	{
		buf_push(OrigString, (char)Temp);
		Token->Value = Temp;
	}

	buf_push(OrigString, '\0');
	Token->Name = OrigString;
	Temp = fgetc(InputStream);
	if(Temp != '\'')
	{
		CASE_ERROR(Token, ERR_UNEXPECTED_CHAR, Temp);
	}
	return;
}
