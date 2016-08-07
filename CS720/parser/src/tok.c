#include <stdlib.h>

#include "tok.h"

#include "string_t.h"

struct tok_t
{
	tok_type_t type;
	string_t *value;
};

tok_t *tok_initialize(void)
{
	tok_t *tok = malloc(sizeof *tok);
	if (tok == NULL)
	{
		return NULL;
	}

	tok->value = string_initialize();
	return tok;
}

void tok_uninitialize(tok_t *tok)
{
	string_uninitialize(tok->value);
	free(tok);
}

tok_type_t tok_get_type(tok_t *tok)
{
	return tok->type;
}

void tok_set_type(tok_t *tok, tok_type_t type)
{
	tok->type = type;
}

string_t *tok_get_value(tok_t *tok)
{
	return tok->value;
}

void tok_set_value(tok_t *tok, string_t *s)
{
	string_copy(tok->value, s);
}

void tok_value_append(tok_t *tok, char c)
{
	string_push_back(tok->value, c);
}

void tok_print(tok_t *tok)
{
#define SWITCH_CASE(x)\
	case x:\
		type = #x;\
		break

	const char *type;
	switch (tok->type)
	{
		SWITCH_CASE(NEWLINE);
		SWITCH_CASE(LPAREN);
		SWITCH_CASE(RPAREN);
		SWITCH_CASE(COLON);
		SWITCH_CASE(COMMA);
		SWITCH_CASE(IDENT);
		SWITCH_CASE(HEXI);
		SWITCH_CASE(DECI);
		SWITCH_CASE(REGISTER);
		SWITCH_CASE(NOP);
		SWITCH_CASE(HALT);
		SWITCH_CASE(FRM);
		SWITCH_CASE(DIS);
		SWITCH_CASE(CLR);
		SWITCH_CASE(INC);
		SWITCH_CASE(DEC);
		SWITCH_CASE(ALM);
		SWITCH_CASE(ADD);
		SWITCH_CASE(SUB);
		SWITCH_CASE(MUL);
		SWITCH_CASE(DIV);
		SWITCH_CASE(MOV);
		SWITCH_CASE(AND);
		SWITCH_CASE(OR);
		SWITCH_CASE(XOR);
		SWITCH_CASE(CMP);
		SWITCH_CASE(JEQ);
		SWITCH_CASE(JGT);
		SWITCH_CASE(JLT);
		SWITCH_CASE(JLE);
		SWITCH_CASE(JGE);
		SWITCH_CASE(TOK_EOF);
		default:
			type = "Unknown";
			break;
	}
#undef SWITCH_CASE

	printf("%s, %s\n", type, string_c_str(tok->value));
}
