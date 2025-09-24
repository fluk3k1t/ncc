#ifndef __H_PARSER_
#define __H_PARSER_

#include "tokenizer.h"

#define LIST_MAX 256
#define PANIC(fmt, ...)                                                     \
    do {                                                                    \
        fprintf(stderr,                                                     \
                "PANIC at %s:%d in %s(): " fmt " rest: %.*s\n",             \
                __FILE__, __LINE__, __func__, ##__VA_ARGS__, _cur->len, _cur->str);\
        fflush(stderr);                                                     \
        exit(1);                                                            \
    } while (0)

#define TRY(p)  \
    ({                                          \
        backtrack = _cur;                       \
        __auto_type _res = (p);                 \
        if (!_res) _cur = backtrack;                 \
        _res;                                    \
    })

#define FAIL(fmt, ...)                                                     \
    do {                                                                    \
        fprintf(stderr,                                                     \
                "FAILED at %s:%d in %s(): " fmt " rest: %.*s\n",             \
                __FILE__, __LINE__, __func__, ##__VA_ARGS__, _cur->len, _cur->str);\
        fflush(stderr);                                                     \
        return NULL;                                                     \
    } while (0)

#define EXPECT(expected)                                                  \
    do {                                                                    \
        if (!consume(expected)) {                                           \
            fprintf(stderr,                                                     \
                "EXPECTED '%s' but '%.*s' at %s:%d in %s(): \n",             \
                expected, _cur->len, _cur->str, __FILE__, __LINE__, __func__);\
            fflush(stderr);     \
            return NULL;                                                    \  
        }                                                                   \
    } while (0)

#define MUST(p)                                                             \
    ({                                                                      \
        __auto_type _v = (p);                                               \
        if (!_v) PANIC("expected " #p "\n");                                \
        _v;                                                                 \
    })                                                                      

// #define TRY(expr) \
//     ({                                                          \
//         token_t *_t = _cur;                                      \
//         __auto_type _v = (expr);                                \
//         if (!_v) {                                              \
//             if (_t != _cur) PANIC("token not recoverd!\n");      \
//             return NULL;                                        \
//         }                                                       \
//         _v;                                                     \
//     })

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
    ND_BLOCK_ITEM_LIST,
    ND_COMPOUND_STATEMENT,
    ND_EXPRESSION_STATEMENT,
    ND_ASSIGNMENT_EXPRESSION,
    ND_ILLEGAL,
    ND_STRUCT_DECLARATION,
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
typedef struct type_t type_t;
typedef struct type_list_t type_list_t;

struct node_t {
    node_t *next;
    node_kind_t kind;
    node_t *lhs, *rhs, *child;
    char *str;
    int len, val;

    union {
        struct {
            node_list_t *block_item_list_opt;
        } compound_statement;

        struct {
            node_t *expression_opt;
        } expression_statement;

        struct {
            node_t *unary_expression, *rec;
            char *assignment_operator;
        } assignment_expression;

        struct {
            decl_list_t *decls;
        } declaration;

        struct {
            decl_t *decl;
            node_t *cs;
        } function_difinition;

        struct {
            type_list_t *specs;
            decl_list_t *struct_declarator_list_opt;
        } struct_declaration;

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

typedef enum {
    VOID, CHAR, SHORT, INT, LONG, STRUCT, UNION, PTR, ARRAY, FUN
} type_kind_t;

struct type_t {
    type_kind_t kind;
    type_t *ptr_to, *ret, *array_to;
    decl_list_t *params;
    char *name;
    int len;

    union {
        struct {
            node_t *ident_opt;
            node_t *struct_declaration_list;
        } struct_or_union_specifier;
    } share;
};

struct type_list_t {
    type_list_t *next;
    type_t *self;
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
decl_t *_direct_declarator(type_t *base);
type_t *_type_specifier();
type_t *_struct_or_union_specifier();
node_t *_struct_declaration_list();
node_t *_struct_declaration();
type_list_t *_specifier_qualifier_list();
decl_list_t *_struct_declarator_list();
decl_t *_struct_declarator();
type_t *_struct_or_union();
type_t *_declaration_specifiers();
decl_list_t *_init_declarator_list(type_t *base);
decl_t *_init_declarator(type_t *base);

node_t *parse(token_t *token);
node_t *_external_declaration();
node_t *_function_definition();
node_t *identifier();
node_t *constant();
node_t *primary_expression();
node_t *multiplicative_expression();
node_t *additive_expression();
node_t *_assignment_expression();
char *_assignment_operator();
node_t *expression();
node_t *initializer();
node_t *_statement();
node_t *_compound_statement();
node_list_t *_block_item_list();
node_t *_block_item();
node_t *_expression_statement();
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
char *oneof(char **ones, int len);
bool consume(char *op);
bool peek(char *op);
bool type(char *c);
void expect(char *op);
bool peek_type(char *name);
char *peek_types(char *names[], int len);
node_t *try_(node_t *(*p)());
void *must(void *(*p)(), char *p_name);
type_t *new_type(type_kind_t kind);


#endif