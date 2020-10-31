
/*


  TOKEN_DIRECTIVE
  TOKEN_OPERATOR
  TOKEN_IDENTIFIER

  TOKEN_LITERAL
  TOKEN_COMMENT ?
  TOKEN_COMMA (',')
  TOKEN_DOT ('.')


  TOKEN_QUOTE (''')
  TOKEN_DQUOTE ('"')
*/

typedef enum token_type
{
	TOK_EOL, TOK_EOF,
	TOK_LABLE, TOK_DIRECTIVE, TOK_LITERAL, TOK_ID, TOK_COMMENT,
	TOK_COMMA, TOK_DOT;
} token_type

typedef enum literal_type
{
	LIT_DEC, LIT_HEX, LIT_BIN, LIT_CHR, LIT_STR;
}

typedef struct token
{
	token_type Type;
	literal_type LiteralType;
	uint32 NumValue;
	char* Name; //aka string value


} token;

next_token()
{

	switch()
	{
	case ' ': case '\t': case '\v':	case '\f': case '\r':
		continue;

	case '0': case '1':	case '2': case '3':	case '4':
	case '5': case '6': case '7': case '8': case '9':

		break;
	case 'a': case 'b': case 'c': case 'd': case 'e':
	case 'f': case 'g': case 'h': case 'i': case 'j':
	case 'k': case 'l': case 'm': case 'n': case 'o':
	case 'p': case 'q': case 'r': case 's': case 't':
	case 'u': case 'v': case 'w': case 'x': case 'y':
	case 'z':
	case 'A': case 'B': case 'C': case 'D': case 'E':
	case 'F': case 'G': case 'H': case 'I': case 'J':
	case 'K': case 'L': case 'M': case 'N': case 'O':
	case 'P': case 'Q': case 'R': case 'S': case 'T':
	case 'U': case 'V': case 'W': case 'X': case 'Y':
	case 'Z': case '_': case '.':

		break;

	case '(': case ')':
		break;

	case '\n':
		break;
	default:

	}
}
