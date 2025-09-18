#ifndef __H_TOKENIZER_
#define __H_TOKENIZER_

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

typedef enum {
    TK_NUM, 
    TK_RESERVED, 
    TK_IDENT, 
    TK_TYPE,
    TK_KEYWORD, 
    TK_EOF,
} token_kind_t;

typedef struct token_t token_t;
struct token_t {
    struct token_t *next;
    token_kind_t kind;
    int val;
    char *str;
    int len;
};

token_t *tokenize(char *str);
token_t *new_token(token_t *cur, token_kind_t kind, char *str);

void show_token(token_t *token);

#endif