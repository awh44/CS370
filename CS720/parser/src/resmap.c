#include "resmap.h"

#include "string_t.h"

static void str_init(string_t **str)
{
	*str = string_initialize();
}

static void str_uninit(string_t **str)
{
	string_uninitialize(*str);
}

static void str_copy(string_t **s1, string_t **s2)
{
	string_concatenate(*s1, *s2);
}

static unsigned short str_equal(string_t **s1, string_t **s2)
{
	return string_compare(*s1, *s2) == 0;
}

static size_t str_hash(string_t **str)
{
	char *key = string_c_str(*str);
    size_t hash_val = 5381;
    char c;

    while ((c = *key++))
    {
        hash_val = ((hash_val << 5) + hash_val) + c;
    }

    return hash_val;
}

static void tok_init(tok_type_t *tok)
{
}

static void tok_uninit(tok_type_t *tok)
{
}

static void tok_copy(tok_type_t *dst, tok_type_t *src)
{
	*dst = *src;
}

MAKE_MAP_C(resmap, string_t *, tok_type_t,
	       str_init, str_uninit, str_copy, str_equal, str_hash,
		   tok_init, tok_uninit, tok_copy)
