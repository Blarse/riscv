#if !defined(LEX_H)
#define LEX_H

typedef enum
{
	TOK_ERROR, TOK_UNDEFINED,
	TOK_EOL, TOK_EOF,
	TOK_COMMA, TOK_PLUS, TOK_MINUS, TOK_OPEN_PAREN, TOK_CLOSE_PAREN,
	TOK_LABLE, TOK_DIRECTIVE, TOK_LITERAL, TOK_IDENTIFIER, TOK_RELOC_FUNC
} TokenType;

typedef enum
{
	LIT_DEC, LIT_HEX, LIT_BIN, LIT_CHR, LIT_STR,
} LiteralType;

typedef enum
{
	ERR_FILE_INPUT, ERR_UNEXPECTED_CHAR, ERR_INT_OVERFLOW,
} LexErrors;


typedef struct Token
{
	TokenType type;
	union
	{
		LiteralType literal_type;
		LexErrors error_code;
	};
	uint32 value;

	char* name;
} Token;

void next_token(Token *token, FILE *input_file);

#endif
