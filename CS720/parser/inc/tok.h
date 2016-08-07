#ifndef _TOK_H_
#define _TOK_H_

#include "string_t.h"

typedef enum
{
	TOK_EOF,
	NEWLINE,
	LPAREN,
	RPAREN,
	COLON,
	COMMA,
	PLUS,
	/*
		LITERAL NUMERALS BELOW
	 */
	HEXI,
	DECI,
	/*
		IDENTIFIER BELOW
	 */
	IDENT,
	/*
	   REGISTER BELOW
	   This is essentially indistinguishable from an IDENT. Will
	   need to be explicitly checked
	 */
	REGISTER,
	/*
		INSTRUCTIONS BELOW
		These are indistinguishable from IDENTs in a strictly
		lexical sense. Need to be treated like "reserved words"
	*/
	NOP,
	HALT,
	FRM,
	DIS,
	CLR,
	INC,
	DEC,
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
	JLE,
	JGE,
} tok_type_t;

typedef struct tok_t tok_t;
tok_t *tok_initialize(void);
void tok_uninitialize(tok_t *tok);
tok_type_t tok_get_type(tok_t *tok);
void tok_set_type(tok_t *tok, tok_type_t type);
string_t *tok_get_value(tok_t *tok);
void tok_value_append(tok_t *tok, char c);
void tok_print(tok_t *tok);
#endif
