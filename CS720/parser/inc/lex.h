#ifndef _LEX_H_
#define _LEX_H_

#include <stdio.h>

#include "status.h"
#include "tok.h"

typedef struct lex_t lex_t;

lex_t *lex_initialize(FILE *stream);
void lex_uninitialize(lex_t *lex);
status_t lex_get_next_token(lex_t *lex, tok_t **tok);

#endif
