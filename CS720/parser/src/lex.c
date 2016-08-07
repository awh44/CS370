#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "lex.h"

#include "resmap.h"
#include "string.h"
#include "tok.h"

struct lex_t
{
	FILE *stream;
	char next;
	resmap_t *reserved;
};

static unsigned short in_range(char c, char lo, char hi);
static unsigned short is_hexi_digit(char c);
static unsigned short is_register(string_t *reg);
static void lex_get_character_tok(lex_t *lex, tok_t *tok, tok_type_t type);

static void lex_skip_whitespace(lex_t *lex);
static void lex_skip_comment(lex_t *lex);
static void lex_get_eof(lex_t *lex, tok_t *tok);
static void lex_get_lparen(lex_t *lex, tok_t *tok);
static void lex_get_rparen(lex_t *lex, tok_t *tok);
static void lex_get_colon(lex_t *lex, tok_t *tok);
static void lex_get_comma(lex_t *lex, tok_t *tok);
static void lex_get_plus(lex_t *lex, tok_t *tok);
static status_t lex_get_hexi(lex_t *lex, tok_t *tok);
static status_t lex_get_deci(lex_t *lex, tok_t *tok);
static void lex_get_ident(lex_t *lex, tok_t *tok);

static char *reserved_words[] =
{
	"nop",
	"halt",
	"clr",
	"inc",
	"dec",
	"frm",
	"dis",
	"alm",
	"add",
	"sub",
	"mul",
	"div",
	"mov",
	"and",
	"or",
	"xor",
	"cmp",
	"jeq",
	"jgt",
	"jlt",
	"jge",
	"jle",
};

static const tok_type_t reserved_types[] =
{
	NOP,
	HALT,
	CLR,
	INC,
	DEC,
	FRM,
	DIS,
	ALM,
	ADD,
	SUB,
	MUL,
	DIV,
	MOV,
	AND,
	OR,
	XOR,
	CMP,
	JEQ,
	JGT,
	JLT,
	JGE,
	JLE,
};

lex_t *lex_initialize(FILE *stream)
{
	lex_t *lex = malloc(sizeof *lex);
	if (lex == NULL)
	{
		goto error0;
	}

	lex->stream = stream;
	lex->reserved = resmap_initialize(64);
	if (lex->reserved == NULL)
	{
		goto error1;
	}

	string_t *str = string_initialize();
	if (str == NULL)
	{
		goto error2;
	}

	map_status_t error;
	size_t i;
	for (i = 0; i < sizeof(reserved_words) / sizeof(*reserved_words); i++)
	{
		string_assign_from_char_array(str, reserved_words[i]);
		error = resmap_put(lex->reserved, str, reserved_types[i]);
		if (error)
		{
			goto error3;
		}
	}
	
	string_uninitialize(str);
	goto success;

error3:
	string_uninitialize(str);
error2:
	resmap_uninitialize(lex->reserved);
error1:
	free(lex);
error0:
	lex = NULL;
	
success:
	return lex;
}

void lex_uninitialize(lex_t *lex)
{
	resmap_uninitialize(lex->reserved);
	free(lex);
}

status_t lex_get_next_token(lex_t *lex, tok_t **tok)
{
	status_t error = SUCCESS;

	*tok = tok_initialize();
	if (*tok == NULL)
	{
		error = OUT_OF_MEM;
		goto exit0;
	}

	//Start by getting the first character to be analyzed
	lex->next = getc(lex->stream);
	lex_skip_whitespace(lex);

	//Comments aren't part of the grammar - just keep eating them in
	//the lexer until we don't have a comment or whitespace
	while (lex->next == ';')
	{
		lex_skip_comment(lex);
		lex_skip_whitespace(lex);
	}

	if (lex->next == EOF)
	{
		lex_get_eof(lex, *tok);
		goto exit0;
	}

	//Get some more literal characters in the next block
	if (lex->next == '(')
	{
		lex_get_lparen(lex, *tok);
		goto exit0;
	}

	if (lex->next == ')')
	{
		lex_get_rparen(lex, *tok);
		goto exit0;
	}

	if (lex->next == ':')
	{
		lex_get_colon(lex, *tok);
		goto exit0;
	}

	if (lex->next == ',')
	{
		lex_get_comma(lex, *tok);
		goto exit0;
	}

	/*
	if (lex->next == '+')
	{
		lex_get_plus(lex, *tok);
		goto exit0;
	}*/

	//Handle the literal numbers in the next two if statements
	if (lex->next == '#')
	{
		error = lex_get_hexi(lex, *tok);
		if (error)
		{
			fprintf(stderr, "Unexpected character in HEXI: %c. Must have form #WXYZ", lex->next);
			goto unexpected;
		}
		goto exit0;
	}

	if (lex->next == '$')
	{
		error = lex_get_deci(lex, *tok);
		if (error)
		{
			fprintf(stderr, "Unexpected character in DECI: %c. Must have form $VWYXZ\n", lex->next);
			goto unexpected;
		}

		goto exit0;
	}
	
	//Everything either IS an identifier or looks like one (registers and instructions),
	//so "get it" as an identifer, but, in the function, it is checked against the keywords
	//and against the possible registers. All of these must start with an alpha or a "-",
	//so make sure before going into the function
	if (isalpha(lex->next) || lex->next == '_')
	{
		lex_get_ident(lex, *tok);
		goto exit0;
	}

	//If we matched nothing else, that means we've got an unexpected character here.
	error = UNEXPECTED_CHAR;
	fprintf(stderr, "Unexpected character: %c\n", lex->next);
unexpected:
	tok_uninitialize(*tok);
	*tok = NULL;

exit0:
	return error;
}

static unsigned short in_range(char c, char lo, char hi)
{
	return c >= lo && c <= hi;
}

static unsigned short is_hexi_digit(char c)
{
	return isdigit(c) || in_range(c, 'A', 'F');
}

static unsigned short is_register(string_t *s)
{
	return string_length(s) == 2 &&
		string_get(s, 0) == 'R' &&
		in_range(string_get(s, 1), '0', '8');
}

static void lex_skip_comment(lex_t *lex)
{
	while (lex->next != '\n' && lex->next != EOF)
	{
		lex->next = getc(lex->stream);
	}
}

static void lex_skip_whitespace(lex_t *lex)
{
	while (isspace(lex->next))
	{
		lex->next = getc(lex->stream);
	}
}

static void lex_get_character_tok(lex_t *lex, tok_t *tok, tok_type_t type)
{
	tok_set_type(tok, type);
	tok_value_append(tok, lex->next);
}

static void lex_get_eof(lex_t *lex, tok_t *tok)
{
	lex_get_character_tok(lex, tok, TOK_EOF);
}

static void lex_get_lparen(lex_t *lex, tok_t *tok)
{
	lex_get_character_tok(lex, tok, LPAREN);
}

static void lex_get_rparen(lex_t *lex, tok_t *tok)
{
	lex_get_character_tok(lex, tok, RPAREN);
}

static void lex_get_colon(lex_t *lex, tok_t *tok)
{
	lex_get_character_tok(lex, tok, COLON);
}

static void lex_get_comma(lex_t *lex, tok_t *tok)
{
	lex_get_character_tok(lex, tok, COMMA);
}

static void lex_get_plus(lex_t *lex, tok_t *tok)
{
	lex_get_character_tok(lex, tok, PLUS);
}

static status_t lex_get_hexi(lex_t *lex, tok_t *tok)
{	
	status_t error = SUCCESS;
	
	tok_set_type(tok, HEXI);

	size_t i;
	for (i = 0; i < 4; i++)
	{
		lex->next = getc(lex->stream);
		if (!is_hexi_digit(lex->next))
		{
			error = UNEXPECTED_CHAR;
			goto exit0;
		}

		tok_value_append(tok, lex->next);
	}

exit0:
	return error;
}

static status_t lex_get_deci(lex_t *lex, tok_t *tok)
{	
	status_t error = SUCCESS;
	tok_set_type(tok, DECI);
	
	size_t i;
	for (i = 0; i < 5; i++)
	{
		lex->next = getc(lex->stream);
		if (!isdigit(lex->next))
		{
			error = UNEXPECTED_CHAR;
			goto exit0;
		}

		tok_value_append(tok, lex->next);
	}

	//Decimal values must be 16-bit. Enforce that here
	int d = atoi(string_c_str(tok_get_value(tok)));
	if (d >= (1 << 16))
	{
		error = UNEXPECTED_CHAR;
		goto exit0;
	}

exit0:
	return error;
}

static void lex_get_ident(lex_t *lex, tok_t *tok)
{
	tok_value_append(tok, lex->next);
	lex->next = getc(lex->stream);

	while (isalnum(lex->next) || lex->next == '_')
	{
		tok_value_append(tok, lex->next);
		lex->next = getc(lex->stream);
	}
	//Got one character that's not an alphanum or a '_', so put it back
	ungetc(lex->next, lex->stream);

	//The registers and instructions look just like identifiers, so need
	//to check the format for registers and the hash map for instructions.
	//If we find a type associated with this identifier, then we use that.
	//Otherwise, it's not a reserved word, so it's just an identifier
	tok_type_t type;
	if (is_register(tok_get_value(tok)))
	{
		type = REGISTER;
	}
	else if (resmap_get(lex->reserved, tok_get_value(tok), &type) != MAP_SUCCESS)
	{
		type = IDENT;
	}

	tok_set_type(tok, type);
}
