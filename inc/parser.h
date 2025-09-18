#ifndef __H_PARSER_
#define __H_PARSER_

#include <tokenizer.h>

typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,
    ND_OR,
    ND_AND, 
    ND_NOT, 
    ND_INC, 
    ND_DEC, 
    ND_POST_INC, 
    ND_POST_DEC, 
    ND_ASSIGN, 
    ND_LVAR, 
    ND_LVAR_DEF,
    ND_GVAR,
    ND_FUNC_DEF,
    ND_NUM, 
    ND_RETURN, 
    ND_IF, 
    ND_WHILE, 
    ND_FOR, 
    ND_BLOCK, 
    ND_CALL, 
    ND_ADDR,
    ND_DEREF,
    ND_MEMBER,
    ND_ALLOW,
} node_kind_t;

typedef struct node_t node_t;
struct node_t {
    node_t *next;
    node_kind_t kind;
    node_t *lhs, *rhs, *child;
    char *str;
    int len, val;
};

typedef enum type_kind_t type_kind_t;
enum type_kind_t {
    VOID, CHAR, SHORT, INT, LONG, STRUCT, PTR, ARRAY, FUNC
};

typedef struct type_t type_t;
struct type_t {
    type_kind_t kind;
    type_t *ptr, *ret, *param;
    char *name;
    int len;
};

node_t *parse(token_t *token);
type_t *parse_type(token_t *token);
type_t *declaration_specifiers();
type_t *pointer();
type_t *type_name();
type_t *abstract_decrarator();
type_t *direct_abstract_declarator();
node_t *constant();
node_t *primary_expression();
node_t *multiplicative_expression();
node_t *additive_expression();
node_t *expression();

void show_node(node_t *node);
void show_node_with_index(node_t *node, int level);
node_t *new_node(node_kind_t kind, node_t *lhs, node_t *rhs);
bool consume(char *op);
bool peek(char *op);
void expect(char *op);
bool type(char *name);

void show_type(type_t *type);
void show_type_with_indent(type_t *type, int level);

#endif