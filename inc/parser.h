#ifndef __H_PARSER_
#define __H_PARSER_

#include "list.h"
#include "tokenizer.h"
#include <stddef.h>

// パーサの方針はあくまで構文をデータ構造に変換することであって、生成コードの必要な情報等々を埋め込まないようにする
// そっちのほうが構成が美しいし、問題個所を切り分けやすいからデバッグもしやすいはず。
// コード量は増えるし、2ステージ間の連携を考える複雑さはあるけど。どっちがいいのかというと、
// 9ccみたいなシンプルな美しさも魅力的だけど、俺はそういう職人芸的なプログラミングに終始し過ぎたくないという
// 思いがあるので。つまりただの好みです
// 例えばdeclarationにおいてstructが定義か宣言かを判定するのではなくてsemanticsに移譲する
// とにかくEBNFを愚直に生成すればよい
// やっぱこの方針なしｗ
// 工数が多すぎてめんどくさいしＣ言語だし

extern token_t *__cur, *__backtrack;
extern token_t *__context_stack[256];
extern int __context_stack_depth;

#define LIST_MAX 256
#define PANIC(fmt, ...)                                                     \
    do {                                                                    \
        fprintf(stderr,                                                     \
                "PANIC at %s:%d in %s(): " fmt " rest: %.*s\n",             \
                __FILE__, __LINE__, __func__, ##__VA_ARGS__, __cur->len, __cur->str);\
        fflush(stderr);                                                     \
        exit(1);                                                            \
    } while (0)

#define TRY(p)  \
({                                          \
    __context_stack[__context_stack_depth++]= __backtrack; \
    __backtrack = __cur;                       \
    __auto_type _res = (p);                 \
    if (!_res) __cur = __backtrack;                 \
    __backtrack = __context_stack[--__context_stack_depth]; \
    _res;                                    \
})

#define FAIL(fmt, ...)                                                     \
    do {                                                                    \
        fprintf(stderr,                                                     \
                "FAILED at %s:%d in %s(): " fmt " rest: %.*s\n",             \
                __FILE__, __LINE__, __func__, ##__VA_ARGS__, __cur->len, __cur->str);\
        fflush(stderr);                                                     \
        return NULL;                                                     \
    } while (0)

#define EXPECT(expected)                                                  \
    do {                                                                    \
        if (!consume(expected)) {                                           \
            fprintf(stderr,                                                     \
                "EXPECTED '%s' but '%.*s' at %s:%d in %s(): \n",             \
                expected, __cur->len, __cur->str, __FILE__, __LINE__, __func__);\
            fflush(stderr);     \
            return NULL;                                                    \  
        }                                                                   \
    } while (0)

#define MUST(p)                                                             \
    ({                                                                      \
        __auto_type _v = (p);                                               \
        if (!_v) FAIL("expected '" #p "'\n");                                \
        _v;                                                                 \
    })                                                                      

#define Many1(T, p) _Many1(ref(T), p)

#define _Many1(T, p) \
({  \
        printf("try to 0 " #p "\n");  \
    __auto_type _e = (TRY(p));   \
    if (!_e) FAIL("---------- many1 tried to " #p);  \
    _LIST(T) *_es = _LIST_NEW(T); \
    _LIST_PUSH(T, _es, _e); \
    do {    \
        printf("try to 1 " #p "\n");  \
        __auto_type _v = (TRY(p));   \
        if (_v) _LIST_PUSH(T, _es, _v);   \
        else break; \ 
    } while (1);    \
    _es;    \
})

#define Many0(T, p) _Many0(ref(T), p)

#define _Many0(T, p)    \
({  \
    _LIST(T) *_es = _LIST_NEW(T); \
    while (1) {    \
        __auto_type _v = (TRY(p));   \
        if (_v) _LIST_PUSH(T, _es, _v);   \
        else break; \ 
    };    \
    _es;    \
})

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
    // struct定義の中の子たち
    ND_STRUCT_DECLARATION,
    // structの型定義そのもの
    ND_STRUCT_DEFINITION,
} node_kind_t;

typedef enum {
    TkChar, TkInt, TkPtr, TkArr,
} TypeKind;

typedef enum {
    Dummy,
} NodeKind;

typedef enum {
    TsVoid, TsChar,
} TypeSpecifier;

typedef enum {
    TqConst, TqVolatile
} TypeQualifier;
DeriveList(ref(TypeQualifier))

typedef struct Node Node;

typedef struct Identifier Identifier;


typedef struct VariableDefinition VariableDefinition;
typedef struct Specifier Specifier;
DeriveList(ref(Specifier))
typedef struct InitDeclarator InitDeclarator;
DeriveList(ref(InitDeclarator))
typedef struct Pointer Pointer;
DeriveList(ref(Pointer))
typedef struct Type Type;
DeriveList(ref(Type))
typedef struct node_t node_t;
typedef struct decl_t decl_t;
typedef struct decl_list_t decl_list_t;
typedef struct type_t type_t;
typedef struct type_list_t type_list_t;

DeriveList(ref(node_t))

DeriveList(ref(Node))
struct Node {
    NodeKind kind;
    Identifier *ident;
};

struct Type {
    TypeKind kind;

    union {
        struct {
            Type *to;
        } ptr;

        struct {
            Type *to;
        } arr;
    } share;
};

struct VariableDefinition {
    List(ref(InitDeclarator)) *init_declarator_list;
};

struct DeclarationSpecifiers {
    List(ref(Specifier)) specifiers;
};

struct Specifier {
    enum {
        SpTypeSpecifier
    } kind;

    union {
        TypeSpecifier type_specifier;
    } share;
};

#define new_specifier(v) _Generic((v), \
    TypeSpecifier: ({   \
        Specifier *s = (Specifier *)calloc(1, sizeof(Specifier));   \
        s->kind = SpTypeSpecifier;  \
        s->share.type_specifier = v;    \
        s;  \
    })  \
)

struct InitDeclarator {
    Type *declarator;
    void *initializer;
};

struct Pointer {
    List(ref(TypeQualifier)) *type_qualifier_list;
};

struct DirectDeclarator {
    enum {
        dm
    } kind;
};

struct DirectDeclaratorSpecifier {
    enum {
        DdsArray, DdsFn,
    } kind;

    union {
        int dummy;
    } share;
};

struct Identifier {
    char *str;
    unsigned int len;
};

struct node_t {
    node_t *next;
    node_kind_t kind;
    node_t *lhs, *rhs, *child;
    char *str;
    int len, val;

    union {
        // struct {
        //     node_t *block_item_list_opt;
        // } compound_statement;

        // struct {
        //     node_t *expression_opt;
        // } expression_statement;

        // struct {
        //     node_list_t *list;
        // } block_item_list;

        // struct {
        //     node_t *unary_expression, *rec;
        //     char *assignment_operator;
        // } assignment_expression;

        // struct {
        //     // おかしい
        //     // type_t *struct_declaration;
        //     decl_list_t *decls;
        // } declaration;

        // struct {
        //     decl_t *decl;
        //     node_t *cs;
        // } function_difinition;

        // struct {
        //     type_list_t *specs;
        //     decl_list_t *struct_declarator_list_opt;
        // } struct_declaration;

        // struct {
        //     node_t *ptr_opt, *direct_decl;
        // } declarator;

        // struct {
        //     node_t *tqs_opt, *ptr_opt;
        // } pointer;

        // struct {
        //     node_t *ident, *braced_declarator;
        //     array_t *array;
        // } direct_declarator;
    } share;
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
            // 苦肉の策
            bool is_definition;
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

node_t *_declaration();
decl_t *_declarator(type_t *base);
type_t *_pointer(type_t *base);
decl_t *_direct_declarator(type_t *base);
type_t *_type_specifier();
type_t *_struct_or_union_specifier();
node_t *_struct_declaration_list();
node_t *_struct_declaration();
type_list_t *_specifier_qualifier_list();
decl_list_t *_struct_declarator_list(type_t *base);
decl_t *_struct_declarator(type_t *base);
type_t *_struct_or_union();
VariableDefinition *declaration();
List(ref(Specifier)) *declaration_specifiers();
List(ref(InitDeclarator)) *init_declarator_list(Type *base);
InitDeclarator *init_declarator(Type *base);
TypeSpecifier *type_specifier();
Type *from_specifiers(List(ref(Specifier)) *specifiers);
Type *from_specifier(Specifier *from);
Type *from_type_specifier(TypeSpecifier type_specifier);
List(ref(Pointer)) *pointer(Type *base);
Pointer *pointer_helper();
List(ref(TypeQualifier)) *type_qualifier_list();
TypeQualifier *type_qualifier();
Type *declarator(Type *base);
Type *direct_declarator(Type *base);
Type *direct_declarator_partial();
type_t *_declaration_specifiers();
decl_list_t *_init_declarator_list(type_t *base);
decl_t *_init_declarator(type_t *base);

List(ref(node_t)) *parse(token_t *token);
node_t *_external_declaration();
node_t *_function_definition();
node_t *_identifier();
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
node_t *_block_item_list();
node_t *_block_item();
node_t *_expression_statement();
decl_list_t *_parameter_list();
decl_t *_parameter_declaration();
decl_t *_abstract_declarator(type_t *base);
decl_t *_direct_abstract_declarator(type_t *base);


node_t *new_node(node_kind_t kind);
node_t *new_node_with(node_kind_t kind, node_t *lhs, node_t *rhs);
char *oneof(char **ones, int len);
bool consume(char *op);
bool peek(char *op);
bool type(char *c);
bool peek_type(char *name);
char *peek_types(char *names[], int len);
type_t *_new_type(type_kind_t kind);
Type *new_type(TypeKind kind);

#endif