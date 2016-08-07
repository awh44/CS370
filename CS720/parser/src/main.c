#include <stdio.h>

#include "lex.h"
#include "status.h"
#include "tok.h"

int main(int argc, char *argv[])
{
	status_t error = SUCCESS;
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s file\n", argv[0]);
		error = CMD_ARGS;
		goto exit0;
	}

	FILE *stream = fopen(argv[1], "r");
	if (stream == NULL)
	{
		fprintf(stderr, "Could not open file %s\n", argv[1]);
		error = FILE_OPEN_ERROR;
		goto exit0;
	}

	lex_t *lex = lex_initialize(stream);
	if (lex == NULL)
	{
		fprintf(stderr, "Could not initialize lexer\n");
		error = OUT_OF_MEM;
		goto exit1;
	}

	tok_t *tok;
	while (!(error = lex_get_next_token(lex, &tok)) && tok_get_type(tok) != TOK_EOF)
	{
		tok_print(tok);
		tok_uninitialize(tok);
	}

	if (!error)
	{
		tok_uninitialize(tok);
	}

	goto exit2;

exit2:
	lex_uninitialize(lex);
exit1:
	fclose(stream);
exit0:
	return error;
}
