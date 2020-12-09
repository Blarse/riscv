#define COLORS 1

#if COLORS
#define COLOR_RESET	  "\033[0m"
#define COLOR_BLACK	  "\033[30m"
#define COLOR_RED	  "\033[31m"
#define COLOR_GREEN	  "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE	  "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN	  "\033[36m"
#define COLOR_WHITE	  "\033[37m"
#define COLOR_BOLDBLACK	  "\033[1m\033[30m"
#define COLOR_BOLDRED	  "\033[1m\033[31m"
#define COLOR_BOLDGREEN	  "\033[1m\033[32m"
#define COLOR_BOLDYELLOW  "\033[1m\033[33m"
#define COLOR_BOLDBLUE	  "\033[1m\033[34m"
#define COLOR_BOLDMAGENTA "\033[1m\033[35m"
#define COLOR_BOLDCYAN	  "\033[1m\033[36m"
#define COLOR_BOLDWHITE	  "\033[1m\033[37m"
#else
#define COLOR_RESET
#define COLOR_BLACK
#define COLOR_RED
#define COLOR_GREEN
#define COLOR_YELLOW
#define COLOR_BLUE
#define COLOR_MAGENTA
#define COLOR_CYAN
#define COLOR_WHITE
#define COLOR_BOLDBLACK
#define COLOR_BOLDRED
#define COLOR_BOLDGREEN
#define COLOR_BOLDYELLOW
#define COLOR_BOLDBLUE
#define COLOR_BOLDMAGENTA
#define COLOR_BOLDCYAN
#define COLOR_BOLDWHITE
#endif

#if defined(DEBUG)


#if PRINT_TOKENS
#define print_token(t) print_token_f((t))

void print_token_f(Token *token)
{
	printf("\tToken <%s> ", tok_to_str[token->type]);

	if(token->type == TOK_LITERAL)
	{
		printf("[val = %u (%d)] [type = %s] [text = '%s']", token->value,
			   token->value, lit_to_str[token->literal_type], token->name);
	}
	else if(token->type == TOK_ERROR)
	{
		printf("[error = %s '%c']", err_to_str[token->error_code], token->value);
	}
	else if(token->type >= TOK_LABEL)
	{
		printf("[text = '%s'] [val = %#x]", token->name, token->value);
	}
	else if(token->type == TOK_EOL)
	{
		printf("\n");
	}

	putchar('\n');
}
#else
#define print_token(t)
#endif

#if PRINT_INPUT_LINES
#define print_input_line(lc, i) print_input_line_f((lc), (i))

void print_input_line_f(uint32 location_counter, FILE *input_file)
{
		fpos_t pos;
		size_t len = 0;
		char *debug_line = NULL;
		char *debug_line_p = NULL;

		fgetpos(input_file, &pos);
		len = getline(&debug_line, &len, input_file);
		debug_line_p = debug_line;
		while(isspace(*(debug_line_p++)));
		debug_line[len-1] = '\0';

		fsetpos(input_file, &pos);
		if(len == -1)

			printf("$%d " COLOR_BOLDGREEN "%s:%d: " COLOR_RESET "EOF:\n",
				   location_counter, input_file_name, line_number);
		else
			printf("$%d "COLOR_BOLDGREEN "%s:%d: " COLOR_RESET "\"%s\":\n",
				   location_counter, input_file_name, line_number, debug_line_p-1);
		free(debug_line);

}
#else
#define print_input_line(lc, i)
#endif


#define dprintf(fmt, ...) printf((fmt), __VA_ARGS__)
#else


#define print_token(t)
#define print_input_line(lc, i)
#define dprintf(fmt, ...)


#endif //#if defined(DEBUG)
