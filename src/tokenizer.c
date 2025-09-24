#include "../inc/tokenizer.h"

const char *puncts[] = {"==", "!=", "<=", ">=", "->", "<", ">", "+", "-", "*", "/", "(", ")", "=", ";", "{", "}", ",", "&", "[", "]", "."};
const char *types[] = {"char", "short", "int", "long", "void", "struct", "union", "typedef", "float", "unsigned"};
const unsigned int types_len = sizeof(types) / sizeof(types[0]);
const char *keywords[] = {"return", "if", "else", "while", "for"};

bool is_alnum(char c) {
    return  ('a' <= c && c <= 'z') ||
            ('A' <= c && c <= 'Z') ||
            ('0' <= c && c <= '9') ||
            (c == '_');
}

const char *match_puncts(char *str) {
    for (int i = 0; i < sizeof(puncts) / sizeof(puncts[0]); i++) {
        if (strncmp(str, puncts[i], strlen(puncts[i])) == 0) {
            return puncts[i];
        }
    }
    return NULL;
}

const char *match_types(char *str) {
    for (int i = 0; i < sizeof(types) / sizeof(types[0]); i++) {
        if (strncmp(str, types[i], strlen(types[i])) == 0 && !is_alnum(str[strlen(types[i])])) {
            return types[i];
        }
    }
    return NULL;
}

const char *match_keywords(char *str) {
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (strncmp(str, keywords[i], strlen(keywords[i])) == 0 && !is_alnum(str[strlen(keywords[i])])) {
            return keywords[i];
        }
    }
    return NULL;
}

token_t *tokenize(char *str) {
    token_t *cur = (token_t *)calloc(1, sizeof(token_t));
    token_t *head = cur;

    while (*str) {
        if (isspace(*str)) {
            str++;
            continue;
        }

        if(*str == '\0') {
            str ++;
            continue;
        }

        if(isdigit(*str)) {
            cur = new_token(cur, TK_NUM, str);
            cur->val = strtol(str, &str, 10);
            cur->len = str - cur->str;
            continue;
        }

        const char *puncts_maybe = match_puncts(str);
        if (puncts_maybe) {
            cur = new_token(cur, TK_RESERVED, str);
            cur->len = strlen(puncts_maybe);
            str += cur->len;
            continue;
        }

        const char *types_maybe = match_types(str);
        if (types_maybe) {
            cur = new_token(cur, TK_TYPE, str);
            cur->len = strlen(types_maybe);
            str += cur->len;
            continue;
        }

        const char *keywords_maybe = match_keywords(str);
        if (keywords_maybe) {
            cur = new_token(cur, TK_KEYWORD, str);
            cur->len = strlen(keywords_maybe);
            str += cur->len;
            continue;
        }

        if (is_alnum(str[0])) {
            cur = new_token(cur, TK_IDENT, str);
            
            char *base = str;
            while(is_alnum(str[0]))
                str ++;
            
            cur->len = str - base;
            continue;
        }

        printf("invalid token \'%c\'\n", *str);
        exit(1);
    }

    new_token(cur, TK_EOF, str);

    return head->next;
}

token_t *new_token(token_t *cur, token_kind_t kind, char *str) {
    token_t *new_token = (token_t *)calloc(1, sizeof(token_t));
    new_token->str = str;
    new_token->kind = kind;
    cur->next = new_token;
    return new_token;
}

void show_token(token_t *cur) {
    while (1) {
        switch (cur->kind) {
            case TK_NUM:
                printf("TK_NUM: '%d'\n", cur->val);
                break;  
            case TK_RESERVED:
                printf("TK_RESERVED: '%.*s'\n", cur->len, cur->str);
                break;
            case TK_IDENT:
                printf("TK_IDENT: '%.*s'\n", cur->len, cur->str);
                break;
            case TK_EOF:
                printf("TK_EOF\n");
                return;
            case TK_TYPE:
                printf("TK_TYPE: '%.*s'\n", cur->len, cur->str);
                break;
            case TK_KEYWORD:
                printf("TK_KEYWORD: '%.*s'\n", cur->len, cur->str);
                break;
            default:
                printf("undefined token kind\n");
                exit(1);
        }
        cur = cur->next;
    }
}