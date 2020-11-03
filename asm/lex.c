#include "lex.h"

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

static void read_hex(token *Token, FILE *InputStream);
static void read_bin(token *Token, FILE *InputStream);
static void read_dec(token *Token, FILE *InputStream);
static void read_string(token *Token, FILE *InputStream);
static void read_comment(FILE *InputStream);
static void read_multiline_comment(FILE *InputStream);

#define CASE_UNDEF(t,v) t->Type = TOK_UNDEFINED; t->NumValue = v;

void next_token(token *Token, FILE *InputStream)
{

repeat:;

	char Cursor = fgetc(InputStream);

	switch(Cursor)
	{
	case '#':
		read_comment(InputStream);
		//goto repeat;
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
			CASE_UNDEF(Token, Cursor);
			return;
		}
	case ' ': case '\t': case '\v':	case '\f': case '\r':
		goto repeat;


	case '-':
		Cursor = fgetc(InputStream);
		if(!isdigit(Cursor))
		{
			//Not a negative number
			//Todo minus sign
			ungetc(Cursor, InputStream);
			Token->Type = TOK_UNDEFINED;
			break;
		}
	case '0':
		Cursor = fgetc(InputStream);
		switch(Cursor)
		{
		case 'x':
			read_hex(Token, InputStream);
			Token->LiteralType = LIT_HEX;
		case 'b':
			read_bin(Token, InputStream);
			Token->LiteralType = LIT_BIN;
		default:
			read_dec(Token, InputStream);
			Token->LiteralType = LIT_DEC;
		}
		Token->Type = TOK_LITERAL;
	case '1': case '2': case '3':	case '4':
	case '5': case '6': case '7': case '8': case '9':
		ungetc(Cursor, InputStream);
		read_dec(Token, InputStream);
		Token->Type = TOK_LITERAL;
		Token->LiteralType = LIT_DEC;
		break;

	case 'a': case 'b': case 'c': case 'd': case 'e':
	case 'f': case 'g': case 'h': case 'i': case 'j':
	case 'k': case 'l': case 'm': case 'n': case 'o':
	case 'p': case 'q': case 'r': case 's': case 't':
	case 'u': case 'v': case 'w': case 'x': case 'y':
	case 'z': case '.':
	case 'A': case 'B': case 'C': case 'D': case 'E':
	case 'F': case 'G': case 'H': case 'I': case 'J':
	case 'K': case 'L': case 'M': case 'N': case 'O':
	case 'P': case 'Q': case 'R': case 'S': case 'T':
	case 'U': case 'V': case 'W': case 'X': case 'Y':
	case 'Z': case '_':

		break;

	case '\'': case '"':
		read_string(Token, InputStream);
		break;

	case ',':
		Token->Type = TOK_COMMA;
		break;

	case '\n':
		Token->Type = TOK_EOL;
		break;

	case EOF:
		Token->Type = ferror(InputStream) ? TOK_UNDEFINED : TOK_EOF;
		break;
	default:
		CASE_UNDEF(Token, Cursor);
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
				ungetc(Temp, InputStream);
				return;
			}
		}
	}
}

static void read_hex(token *Token, FILE *InputStream)
{
	int32 Temp;
	int32 Result = 0;
	while((Temp = fgetc(InputStream)) != EOF)
	{
		if(isxdigit(Temp))
		{
			Result = (Result << 4)	+ CharToInt[Temp];
		}
		else
		{
			Token->NumValue = Result;
			printf("%s result: %d\n", __func__, Result);
			ungetc(Temp, InputStream);
			return;
		}
	}
}

static void read_bin(token *Token, FILE *InputStream)
{
	int32 Temp;
	int32 Result = 0;
	while((Temp = fgetc(InputStream)) != EOF)
	{
		if(Temp == '0' || Temp == '1')
		{
			Result = (Result << 1)	+ CharToInt[Temp];
		}
		else
		{
			Token->NumValue = Result;
			printf("%s result: %d\n", __func__, Result);
			ungetc(Temp, InputStream);
			return;
		}
	}
}

static void read_dec(token *Token, FILE *InputStream)
{
	int32 Temp;
	int32 Result = 0;
	while((Temp = fgetc(InputStream)) != EOF)
	{
		if(isdigit(Temp))
		{
			Result = (Result * 10)	+ CharToInt[Temp];
		}
		else
		{
			Token->NumValue = Result;
			printf("%s result: %d\n", __func__, Result);
			ungetc(Temp, InputStream);
			return;
		}
	}
}

static void read_string(token *Token, FILE *InputStream)
{

}
