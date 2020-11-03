#if !defined(LEX_H)
#define LEX_H
/*

  TOKEN_DIRECTIVE
  TOKEN_OPERATOR
  TOKEN_IDENTIFIER

  TOKEN_LITERAL
  TOKEN_COMMA (',')

  TOKEN_QUOTE (''')
  TOKEN_DQUOTE ('"')
*/

typedef enum
{
	TOK_EOL, TOK_EOF, TOK_COMMA, TOK_UNDEFINED,
	TOK_LABLE, TOK_DIRECTIVE, TOK_LITERAL, TOK_IDENTIFIER
} token_type;

typedef enum
{
	LIT_DEC, LIT_HEX, LIT_BIN, LIT_CHR, LIT_STR
} literal_type;

typedef struct token
{
	token_type Type;
	literal_type LiteralType;
	uint32 NumValue;
	char* Name;
} token;

void next_token(token *Token, FILE *InputFile);

#endif
