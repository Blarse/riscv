#if !defined(LEX_H)
#define LEX_H

typedef enum
{
	TOK_ERROR, TOK_UNDEFINED,
	TOK_EOL, TOK_EOF, TOK_COMMA,
	TOK_LABLE, TOK_DIRECTIVE, TOK_LITERAL, TOK_IDENTIFIER
} token_type;

typedef enum
{
	LIT_DEC, LIT_HEX, LIT_BIN, LIT_CHR, LIT_STR,
} literal_type;

typedef enum
{
	ERR_FILE_INPUT, ERR_UNEXPECTED_CHAR, ERR_INT_OVERFLOW,
} lex_errors;


typedef struct token
{
	token_type Type;
	union
	{
		literal_type LiteralType;
		lex_errors ErrorCode;
	};
	uint32 Value;

	char* Name;
} token;

void next_token(token *Token, FILE *InputFile);

#endif
