#include "../inc/parser.h"

#include <stdio.h>
#include "../inc/tokenizer.h"

token_t *__cur = NULL;
token_t *__backtrack = NULL;
token_t *__context_stack[256] = {NULL};
int __context_stack_depth = 0;

node_t *parse(token_t *token) {
    __cur = token;
    __backtrack = __cur;

    node_t *exd = _external_declaration();
    
    return exd;
}

node_t *_external_declaration() {
    node_t *fd = TRY(_function_definition());
    if (fd) return fd;

    // PANIC("koko");

    node_t *decl = _declaration();
    if (decl) return decl;
    
    PANIC("no matching syntax!\n");
}

node_t *_function_definition() {
    type_t *tspecs = _declaration_specifiers();
    if (!tspecs) FAIL("toplevel");

    decl_t *decl = _declarator(tspecs);
    if (!decl) { FAIL("EXPECTed '_declarator()'\n"); }

    node_t *cs = _compound_statement();
    if (!cs) FAIL("expected compound statement\n");

    node_t *fd = new_node(ND_FUNCTION_DEFINITION);
    fd->share.function_difinition.decl = decl;
    fd->share.function_difinition.cs = cs;

    return fd;
}

node_t *identifier() {
    if (__cur->kind == TK_IDENT) {
        node_t *ident = new_node(ND_IDENT);
        ident->str = __cur->str;
        ident->len = __cur->len;

        __cur = __cur->next;
        return ident;
    } else {
        return NULL;
    }
}

node_t *constant() {
    if (__cur->kind != TK_NUM) {
        printf("constant(): EXPECT number but\n");
        exit(1);
    }

    node_t *node = (node_t *)calloc(1, sizeof(node_t));
    node->kind = ND_NUM;
    node->val = __cur->val;
    
    __cur = __cur->next;

    return node;
}

node_t *primary_expression() {
    node_t *ident = identifier();
    if (ident) return ident;

    if (__cur->kind == TK_NUM) {
        return constant();
    } else if (consume("(")) {
        node_t *e = expression();
        EXPECT(")");
        return e;
    } else {
        PANIC("primary_expression: unimplemented! %s\n", __cur->str);
    }
}

node_t *multiplicative_expression() {
    node_t *cur = primary_expression();

    while (1) {
        if (consume("*")) {
            cur = new_node_with(ND_MUL, cur, primary_expression());
        } else if (consume("/")) {
            cur = new_node_with(ND_DIV, cur, primary_expression());
        } else {
            break;
        }
    }

    return cur;
}

node_t *additive_expression() {
    node_t *cur = multiplicative_expression();

    while (1) {
        if (consume("+")) {
            cur = new_node_with(ND_ADD, cur, multiplicative_expression());
        } else if (consume("-")) {
            cur = new_node_with(ND_SUB, cur, multiplicative_expression());
        } else {
            break;
        }
    }

    return cur;
}

node_t *_assignment_expression() {
    // TODO: 本来はunary-expression
    node_t *exp = additive_expression();
    if (!exp) return NULL;

    // TODO: assignment_expressionがただのexpressionを返してしまっていいのか
    char *aop = _assignment_operator();
    if (!aop) return exp;

    node_t *aexp_rec = _assignment_expression();
    if (!aexp_rec) { PANIC("expected assignment expression \n"); }

    node_t *aexp = new_node(ND_ASSIGNMENT_EXPRESSION);
    aexp->share.assignment_expression.unary_expression = exp;
    aexp->share.assignment_expression.assignment_operator = aop;
    aexp->share.assignment_expression.rec = aexp_rec;

    return aexp;
}

char *assignment_operators[] = {"="};
const int assignment_operators_len = sizeof(assignment_operators) / sizeof(assignment_operators[0]);

char *_assignment_operator() {
    return oneof(assignment_operators, assignment_operators_len);
}

node_t *expression() {
    node_t *e = _assignment_expression();
    return e;
}

node_t *_declaration() {
    type_t *decl_specs = _declaration_specifiers();
    if (!decl_specs) return NULL;

    // TODO: optionalのハンドリング
    // TODO: tryのバックトラックそのものの退避が必要な希ガス
    decl_list_t *init_dectr_list_opt = TRY(_init_declarator_list(decl_specs));
    // if (!init_dectr_list) { return  }

    EXPECT(";");

    node_t *decl = new_node(ND_DECLARATION);
    decl->share.declaration.decls = init_dectr_list_opt;

    return decl;
}

type_t *_declaration_specifiers() {
    while (1) {
        type_t *tspec = _type_specifier();
        if (tspec) {
            return tspec;
        }

        break;
    }

    return NULL;
}

decl_list_t *_init_declarator_list(type_t *base) {
    decl_list_t *cur = (decl_list_t *)calloc(1, sizeof(decl_list_t));
    decl_list_t *head = cur;

    do {
        decl_t *i = _init_declarator(base);
        if (!i) { FAIL("EXPECTed init-declarator"); }
        decl_list_t *next = (decl_list_t *)calloc(1, sizeof(decl_list_t));
        next->self = i;
        cur->next = next;
        cur = next;
    } while (consume(","));

    return head->next;
}

decl_t *_init_declarator(type_t *base) {
    decl_t *dectr = _declarator(base);
    if (!dectr) return NULL;

    if (consume("=")) {
        node_t *init = initializer();
        if (!init) { PANIC("EXPECTed initializer"); }
        dectr->init = init;
    }

    return dectr;
}

type_t *_type_specifier() {
    if (type("void"))           return new_type(VOID);
    else if (type("char"))      return new_type(CHAR);
    else if (type("short"))     return new_type(SHORT);
    else if (type("int"))       return new_type(INT);
    else if (type("long"))      return new_type(LONG);
    else if (type("float"))     PANIC("todo!()\n");
    else if (type("double"))    PANIC("todo!()\n");
    else if (type("signed"))    PANIC("todo!()\n");
    else if (type("unsigned"))  PANIC("todo!()\n");
    else {
        type_t *st = TRY(_struct_or_union_specifier());
        if (st) return st;

        return NULL;
    }
}

type_t *_struct_or_union_specifier() {
    token_t *_s = __cur;
    type_t *st = _struct_or_union();
    if (!st) return st;

    node_t *ident_opt = identifier();
    
    if (consume("{")) {
        node_t *stdecls = _struct_declaration_list();
        if (!stdecls) FAIL("expected struct declaration list");

        EXPECT("}");
        
        st->share.struct_or_union_specifier.ident_opt = ident_opt;
        st->share.struct_or_union_specifier.struct_declaration_list = stdecls; 

        return st;
    } else {
        // if (!ident_opt) PANIC("expected identifier");
        PANIC("");
    }
}

// struct-declaration-list := struct-declaration+
node_t *_struct_declaration_list() {
    return _struct_declaration();
}

// struct-declaration := specifier-qualifier-list struct-declarator-list? ";"
//                     | static_assert-declaration
node_t *_struct_declaration() {
    type_list_t *specs = _specifier_qualifier_list();
    if (!specs) return NULL;

    decl_list_t *decls = _struct_declarator_list();

    EXPECT(";");

    node_t *stdecl = new_node(ND_STRUCT_DECLARATION);
    stdecl->share.struct_declaration.specs = specs;
    stdecl->share.struct_declaration.struct_declarator_list_opt = decls;

    return stdecl;
}

// specifier-qualifier-list := (type-specifier | type-qualifier)+
type_list_t *_specifier_qualifier_list() {
    type_list_t *head = NULL;
    type_list_t **tail = &head;

    while (1) {
        type_t *tspec = _type_specifier();
        if (!tspec) break;

        type_list_t *ts = (type_list_t *)calloc(1, sizeof(type_list_t));
        ts->self = tspec;
        *tail = ts;
        tail = &ts->next;
    }

    return head;
}

// struct-declarator-list := struct-declarator ("," struct-declarator)*
decl_list_t *_struct_declarator_list() {
    decl_t *decl = _struct_declarator();
    if (!decl) return NULL;
    
    decl_list_t *decls = (decl_list_t *)calloc(1, sizeof(decl_list_t));
    decls->self = decl;

    return decls;
}

// struct-declarator := declarator
//                    | declarator? : constant-expression
decl_t *_struct_declarator() {
    return _declarator(NULL);
}

// struct-or-union := "struct" | "union"
type_t *_struct_or_union() {
    if      (type("struct"))    return new_type(STRUCT);
    else if (type("union"))     return new_type(UNION);
    else                           return NULL; 
}

decl_t *_declarator(type_t *base) {
    token_t *__backtrack = __cur;

    type_t *ptr_opt = _pointer(base);
    decl_t *direct_decl = _direct_declarator(ptr_opt);

    if (!direct_decl) {
        __cur = __backtrack;
        return NULL;
    }

    return direct_decl;
}

decl_t *_direct_declarator(type_t *base) {
    token_t *b = __cur;
    type_t *braced = NULL;

    node_t *ident = identifier();

    printf("ident\n");
    if (!ident) {
        printf("no %s\n", __cur->str);
        if (consume("(")) {
            decl_t *dectr = _declarator(NULL);
            if (!dectr) {
                __cur = b;
                return NULL;
            }

            braced = dectr->decl;
            // 冗長かも?
            ident = dectr->ident;
            EXPECT(")");
        } else {
            // abstに委譲
            return NULL;
        }
    }else printf("aru\n");

    while (1) {
        if (consume("[")) {
            EXPECT("]");
            type_t *arr = new_type(ARRAY);
            arr->array_to = base;
            base = arr;
        } else if (consume("(")) {
            decl_list_t *ps = _parameter_list();
            EXPECT(")");
            type_t *fn = new_type(FUN);
            fn->params = ps;
            fn->ret = base;
            base = fn;
        } else {
            break;
        }
    }

    if (braced) {
        type_t *tail = braced;
        while (1) {
            if (tail->ptr_to || tail->array_to || tail->ret) {
                if (tail->kind == PTR) {
                    tail = tail->ptr_to;
                } else if (tail->kind == ARRAY) {
                    tail = tail->array_to;
                } else if (tail->kind == FUN) {
                    tail = tail->ret;
                } else {
                    PANIC("unreachable");
                }
            } else {
                break;
            }
        }

        if (tail->kind == PTR) {
            tail->ptr_to = base;
        } else if (tail->kind == ARRAY) {
            tail->array_to = base;
        } else if (tail->kind == FUN) {
            tail->ret = base;
        } else {
            PANIC("unreachable");
        }

        base = braced;
    }

    // decl_tはただのtype_tとidentのタプルなので動的確保しなくてもいいが混在するとややこしいので
    decl_t *d = (decl_t *)calloc(1, sizeof(decl_t));
    d->decl = base;
    d->ident = ident;

    return d;
}

decl_list_t *_parameter_list() {
    decl_list_t *head = NULL;
    decl_list_t **tail = &head;

    decl_t *p = _parameter_declaration();
    if (p) {
        do {
            decl_list_t *decl = (decl_list_t *)calloc(1, sizeof(decl_list_t));
            decl->self = p;

            *tail = decl;
            tail = &decl->next;
        } while (consume(",") && (p = _parameter_declaration()));
    }

    return head;
}

decl_t *_parameter_declaration() {
    type_t *tspecs = _declaration_specifiers();
    if (!tspecs) return NULL;

    decl_t *dectr = TRY(_declarator(tspecs));
    if (dectr) return dectr;

    decl_t *abst_dectr = _abstract_declarator(tspecs);

    if (abst_dectr) return abst_dectr;

    // return tspecs;
    // decl_t *abst_dectr = _abstract_declarator(tspecs);
    // if (!abst_dectr) { PANIC("EXPECTed abstract-declarator()"); }

    PANIC("mendounanode hitaiou");
}

decl_t *_abstract_declarator(type_t *base) {
    type_t *ptr = _pointer(base);
    decl_t *direct_abst_dectr = _direct_abstract_declarator(ptr);
    
    if (!ptr) {
        if (!direct_abst_dectr) { PANIC("EXPECTed direct-abstract-declarator()"); }
    } else {
        if (!direct_abst_dectr) { 
            decl_t *ptr_decl = (decl_t *)calloc(1, sizeof(decl_t));
            ptr_decl->decl = ptr;
            return ptr_decl;
        }
    }

    return direct_abst_dectr;
}

decl_t *_direct_abstract_declarator(type_t *base) {
    type_t *braced = NULL;

    if (consume("(")) {
        // TODO: つまりdecl_t ではなくtype_tで事足りる
        braced = _abstract_declarator(NULL)->decl;
        EXPECT(")");
    }

    while (1) {
        if (consume("[")) {
            EXPECT("]");
            type_t *arr = new_type(ARRAY);
            arr->array_to = base;
            base = arr;
        } else if (consume("(")) {
            decl_list_t *ps = _parameter_list();
            EXPECT(")");
            type_t *fn = new_type(FUN);
            fn->params = ps;
            fn->ret = base;
            base = fn;
        } else {
            break;
        }
    }

    if (braced) {
        type_t *tail = braced;
        while (1) {
            if (tail->ptr_to || tail->array_to || tail->ret) {
                if (tail->kind == PTR) {
                    tail = tail->ptr_to;
                } else if (tail->kind == ARRAY) {
                    tail = tail->array_to;
                } else if (tail->kind == FUN) {
                    tail = tail->ret;
                } else {
                    // show_type(tail);
                    PANIC("unreachable");
                }
            } else {
                break;
            }
        }

        if (tail->kind == PTR) {
            tail->ptr_to = base;
        } else if (tail->kind == ARRAY) {
            tail->array_to = base;
        } else if (tail->kind == FUN) {
            tail->ret = base;
        } else {
            PANIC("unreachable");
        }

        base = braced;
    }

    decl_t *d = (decl_t *)calloc(1, sizeof(decl_t));
    d->decl = base;
    // d->ident = ident;

    return d;
}

type_t *_pointer(type_t *base) {
    type_t *head = base;

    while (consume("*")) {
        type_t *ptr = new_type(PTR);
        ptr->ptr_to = base;
        base = ptr;
    }

    return base;
}

extern const char *types[];
extern const unsigned int types_len;

// (6.7.9)
node_t *initializer() {
    // tmp
    return _assignment_expression();
}

// node_t *_function_difinition() {
//     type_t *tspecs = _declaration_specifiers();
//     if (!tspecs) return NULL;

//     decl_t *dectr = _declarator(tspecs);
//     if (!dectr) { PANIC("EXPECTed '_declarator()'\n"); }

//     node_t *fd = new_node(ND_FUNCTION_DEFINITION);
//     fd->share.function_difinition.decl = dectr;

//     return fd;
// }

node_t *_statement() {
    node_t *cs = _compound_statement();
    if (cs) return cs;

    // TODO: この依存をなくすにはバックトラックが必要
    if (peek("}")) return NULL;

    node_t *es = _expression_statement();
    if (es) return es;

    PANIC("unreachable!");
}

node_t *_compound_statement() {
    if (consume("{")) {
        node_list_t *bs = _block_item_list();
        EXPECT("}");
        node_t *cps = new_node(ND_COMPOUND_STATEMENT);
        cps->share.compound_statement.block_item_list_opt = bs;
        return cps;
    } else {
        return NULL;
    }
}

node_list_t *_block_item_list() {
    node_list_t *cur = (node_list_t *)calloc(1, sizeof(node_list_t));
    node_list_t *head = cur;

    while (1) {
        node_t *item = _block_item();
        if (!item) break;

        node_list_t *next = (node_list_t *)calloc(1, sizeof(node_list_t));
        next->self = item;

        cur->next = next;
        cur = next;
    }

    return head->next;
}

node_t *_block_item() {
    node_t *decl = _declaration();
    if (decl) return decl;

    node_t *stmt = _statement();
    if (stmt) return stmt;

    // 空{}用
    return NULL;
}

// expressionがNULLの場合でも';'を必須とするので呼び出し側での優先順位は最下位
node_t *_expression_statement() {
    node_t *exp = expression();
    EXPECT(";");

    node_t *es = new_node(ND_EXPRESSION_STATEMENT);
    es->share.expression_statement.expression_opt = exp;

    return es;
}

char *oneof(char **ones, int len) {
    for (int i = 0; i < len; i ++) {
        if (consume(ones[i])) {
            return ones[i];
        }
    }

    return NULL;
} 

bool consume(char *op) {
    if (!__cur) PANIC("current token is null\n");

    if (__cur->kind == TK_RESERVED || __cur->kind == TK_IDENT) {
        // printf("compare %s to %.*s\n", op, __cur->len, __cur->str);
        if (__cur->len == strlen(op) && strncmp(__cur->str, op, __cur->len) == 0) {
            // printf("match\n");
            __cur = __cur->next;
            return true;
        }
    }

    return false;
}

bool peek(char *op) {
    if (!__cur) PANIC("current token is null\n");

    if (__cur->kind == TK_RESERVED || __cur->kind == TK_IDENT) {
        if (__cur->len == strlen(op) && strncmp(__cur->str, op, __cur->len) == 0) {
            return true;
        }
    }

    return false;
}

bool type(char *c) {
    if (!__cur) PANIC("current token is null\n");

    // printf("compare %s to %s\n", c, __cur->str);

    if (__cur->kind == TK_TYPE) {
        if (__cur->len == strlen(c) && strncmp(__cur->str, c, __cur->len) == 0) {
            __cur = __cur->next;
            return true;
        }
    }

    return false;
}

void expect(char *op) {
    if (__cur->kind == TK_RESERVED || __cur->kind == TK_IDENT) {
        if (__cur->len == strlen(op) && strncmp(__cur->str, op, __cur->len) == 0) {
            __cur = __cur->next;
            return;
        }
    }

    printf("EXPECT '%s' but '%.*s'\n", op, __cur->len, __cur->str);
    exit(1);
}

char *peek_types(char *names[], int len) {
    for (int i = 0; i < len; i++) {
        // printf("compare '%s' to '%.*s'\n", names[i], cur->len, cur->str);
        if (peek_type(names[i])) {
            return names[i];
        }
    }

    return NULL;
}

bool peek_type(char *name) {
    if (__cur->kind == TK_TYPE) {
        if (__cur->len == strlen(name) && strncmp(__cur->str, name, __cur->len) == 0) {   
            return true;
        }
    }
    
    return false;
}

node_t *new_node_with(node_kind_t kind, node_t *lhs, node_t *rhs) {
    node_t *node = (node_t *)calloc(1, sizeof(node_t));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;

    return node;
}

node_t *new_node(node_kind_t kind) {
    node_t *node = (node_t *)calloc(1, sizeof(node_t));
    node->kind = kind; 

    return node;
}

// パースが失敗してもトークンを消費しないパーサにtryは必要ない
node_t *try_(node_t *(*p)()) {
    token_t *___backtrack = __cur;
    node_t *res = p();

    if (res) {
        return res;
    } else {
        // printf("try fial\n");
        __cur = ___backtrack;
        return NULL;
    }
}

type_t *new_type(type_kind_t kind) {
    type_t *type = (type_t *)calloc(1, sizeof(type_t));
    type->kind = kind;

    return type;
}

void *must(void *(*p)(), char *p_name) {
    void *res = p();
    if (!res) {
        PANIC("%s: failed\n", p_name);
    }

    return res;
}