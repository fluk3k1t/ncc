#ifndef __H_PARSER_
#define __H_PARSER_

#include <tokenizer.h>

#define LIST_MAX 256
#define PANIC(fmt, ...)                                                     \
    do {                                                                    \
        fprintf(stderr,                                                     \
                "PANIC at %s:%d in %s(): " fmt "\n",                        \
                __FILE__, __LINE__, __func__, ##__VA_ARGS__);               \
        fflush(stderr);                                                     \
        exit(1);                                                            \
    } while (0)

// #define MUST(p) 

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
    ND_IDENT,
    ND_DECLARATION,
    // declaration_specifiers -> lhs, init_declarator_list -> rhs(opt(NULL))
    ND_TYPE_SPECIFIER,
    ND_TYPE_QUALIFIER,
    ND_DECLARATOR,
    // pointer(opt) -> lhs, direct_declarator -> rhs
    ND_POINTER,
    ND_DIRECT_DECLARATOR,
    // top: identifier -> lhs, direct_declarator(rec) -> array
    ND_INIT_DECLARATOR,
    // declarator -> lhs, init(opt) -> rhs
    ND_FUNCTION_DEFINITION,
    ND_PARAMETER_DECLARATION,
} node_kind_t;

typedef struct array_t array_t;
struct array_t {
    array_t *next;
    int len;
};

typedef struct node_t node_t;
typedef struct node_list_t node_list_t;
typedef struct decl_t decl_t;
typedef struct decl_list_t decl_list_t;

struct node_t {
    node_t *next;
    node_kind_t kind;
    node_t *lhs, *rhs, *child;
    char *str;
    int len, val;

    union {
        struct {
            decl_list_t *decls;
        } declaration;

        struct {
            decl_t *decl;
        } function_difinition;

        struct {
            node_t *ptr_opt, *direct_decl;
        } declarator;

        struct {
            node_t *tqs_opt, *ptr_opt;
        } pointer;

        struct {
            node_t *ident, *braced_declarator;
            array_t *array;
        } direct_declarator;
    } share;
};

struct node_list_t {
    node_t *self;
    node_list_t *next;
};

typedef enum type_kind_t type_kind_t;
enum type_kind_t {
    VOID, CHAR, SHORT, INT, LONG, STRUCT, PTR, ARRAY, FUN
};


typedef struct type_t type_t;
struct type_t {
    type_kind_t kind;
    type_t *ptr_to, *ret, *array_to;
    decl_list_t *params;
    char *name;
    int len;
};

struct decl_t {
    type_t *decl;
    node_t *ident, *init;
};

struct decl_list_t {
    decl_t *self;
    decl_list_t *next;
};

node_t *_declaration();
decl_t *_declarator(type_t *base);
type_t *_pointer(type_t *base);
decl_t *_direct_declarator();
type_t *_type_specifier();
type_t *_declaration_specifiers();
decl_list_t *_init_declarator_list(type_t *base);
decl_t *_init_declarator(type_t *base);

node_t *parse(token_t *token);
node_t *identifier();
node_t *constant();
node_t *primary_expression();
node_t *multiplicative_expression();
node_t *additive_expression();
node_t *expression();
node_t *initializer();
decl_list_t *_parameter_list();
decl_t *_parameter_declaration();
decl_t *_abstract_declarator(type_t *base);
decl_t *_direct_abstract_declarator(type_t *base);

void show_node(node_t *node);
void show_node_with_indent(node_t *node, int level);
void show_array_with_indent(array_t *array, int level);
void show_type(type_t *type);
node_t *new_node(node_kind_t kind);
node_t *new_node_with(node_kind_t kind, node_t *lhs, node_t *rhs);
bool consume(char *op);
bool peek(char *op);
void expect(char *op);
bool peek_type(char *name);
char *peek_types(char *names[], int len);
node_t *try_(node_t *(*p)());
void *must(void *(*p)(), char *p_name);
type_t *new_type(type_kind_t kind);


#endif