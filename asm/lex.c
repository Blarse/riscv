#include <limits.h>
#include "lex.h"

static void read_hex(token *Token, FILE *InputStream);
static void read_bin(token *Token, FILE *InputStream);
static void read_dec(token *Token, FILE *InputStream);
static void read_string(token *Token, FILE *InputStream);
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

#define CASE_ERROR(t,e,v) do{t->Type = TOK_ERROR; t->ErrorCode = e; t->NumValue = v;}while(0)

void next_token(token *Token, FILE *InputStream)
{

	Token->Type = TOK_UNDEFINED;
	Token->NumValue = 0;
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
		Cursor = fgetc(InputStream);
		ungetc(Cursor, InputStream);
		if(!isdigit(Cursor) || Token->NumValue == -1)
		{
			//Not a negative number
			//Todo minus sign
			CASE_ERROR(Token, ERR_UNEXPECTED_CHAR, Cursor);

			break;
		}
		Token->NumValue = -1;
		goto repeat;

	case '0':
		Cursor = fgetc(InputStream);

		switch(Cursor)
		{
		case 'x':
			read_hex(Token, InputStream);
			break;
		case 'b':
			read_bin(Token, InputStream);
			break;
		default:
			ungetc(Cursor, InputStream);
			read_dec(Token, InputStream);
		}

		break;

	case '1': case '2': case '3':	case '4':
	case '5': case '6': case '7': case '8': case '9':
		ungetc(Cursor, InputStream);
		read_dec(Token, InputStream);
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

	if(Token->NumValue) //Negative number (max: 2147483647)
	{
		while((Temp = fgetc(InputStream)) != EOF)
		{
			if(isxdigit(Temp))
			{
				ResultNext = (Result << 4) + CharToInt[Temp];

				if(ResultNext > (uint32)INT_MAX+1)
				{
					Token->Type = TOK_ERROR;
					Token->ErrorCode = ERR_INT_OVERFLOW;
				}

				Result = ResultNext;
			}
			else
			{
				ungetc(Temp, InputStream);
				Token->NumValue = Result * -1;
				return;
			}
		}
	}
	else //Positive number (max: 4294967295)
	{
		while((Temp = fgetc(InputStream)) != EOF)
		{
			if(isxdigit(Temp))
			{
				ResultNext = (Result << 4) + CharToInt[Temp];

				if(ResultNext < Result)
				{
					Token->Type = TOK_ERROR;
					Token->ErrorCode = ERR_INT_OVERFLOW;
				}

				Result = ResultNext;
			}
			else
			{
				ungetc(Temp, InputStream);
				Token->NumValue = Result;
				return;
			}
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

	if(Token->NumValue) //Negative number (max: 2147483647)
	{
		while((Temp = fgetc(InputStream)) != EOF)
		{
			if(Temp == '0' || Temp == '1')
			{
				ResultNext = (Result << 1) + CharToInt[Temp];

				if(ResultNext > (uint32)INT_MAX+1)
				{
					Token->Type = TOK_ERROR;
					Token->ErrorCode = ERR_INT_OVERFLOW;
				}

				Result = ResultNext;
			}
			else
			{
				ungetc(Temp, InputStream);
				Token->NumValue = Result * -1;
				return;
			}
		}

	}
	else //Positive number (max: 4294967295)
	{
		while((Temp = fgetc(InputStream)) != EOF)
		{
			if(Temp == '0' || Temp == '1')
			{
				ResultNext = (Result << 1) + CharToInt[Temp];

				if(ResultNext < Result)
				{
					Token->Type = TOK_ERROR;
					Token->ErrorCode = ERR_INT_OVERFLOW;
				}

				Result = ResultNext;
			}
			else
			{
				ungetc(Temp, InputStream);
				Token->NumValue = Result;
				return;
			}
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


	if(Token->NumValue) //Negative number (max: 2147483647)
	{
		while((Temp = fgetc(InputStream)) != EOF)
		{
			if(isdigit(Temp))
			{
				ResultNext = (Result * 10) + CharToInt[Temp];

				if(ResultNext > (uint32)INT_MAX+1)
				{
					Token->Type = TOK_ERROR;
					Token->ErrorCode = ERR_INT_OVERFLOW;
				}

				Result = ResultNext;
			}
			else
			{
				ungetc(Temp, InputStream);
				Token->NumValue = Result * -1;
				return;
			}
		}

	}
	else //Positive number (max: 4294967295)
	{
		while((Temp = fgetc(InputStream)) != EOF)
		{
			if(isdigit(Temp))
			{
				ResultNext = (Result * 10) + CharToInt[Temp];

				if(ResultNext < Result)
				{
					Token->Type = TOK_ERROR;
					Token->ErrorCode = ERR_INT_OVERFLOW;
				}

				Result = ResultNext;
			}
			else
			{
				ungetc(Temp, InputStream);
				Token->NumValue = Result;
				return;
			}
		}

	}
}

static void read_string(token *Token, FILE *InputStream)
{

}
