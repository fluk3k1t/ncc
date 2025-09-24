#include "../inc/parser.h"

#include <stdio.h>
#include "../inc/tokenizer.h"

static token_t *_cur = NULL;
static token_t *backtrack = NULL;

void put_indent(int level) {
    for (int i = 0; i < level; i++) {
        printf("  ");
    }
}

void show_node(node_t *node) {
    show_node_with_indent(node, 0);
}

void show_node_with_indent(node_t *node, int level) {
    if (!node) {
        printf("show_node_with_indent: node is NULL\n");
        exit(1);
    }

    switch (node->kind) {
        case ND_NUM:
            put_indent(level); printf("ND_NUM: %d\n", node->val);
            break;
        case ND_ADD:
            put_indent(level); printf("ND_ADD: \n");
            show_node_with_indent(node->lhs, level + 1);
            show_node_with_indent(node->rhs, level + 1);
            break;
        case ND_SUB:
            put_indent(level); printf("ND_SUB: \n");
            show_node_with_indent(node->lhs, level + 1);
            show_node_with_indent(node->rhs, level + 1);
            break;
        case ND_MUL:
            put_indent(level); printf("ND_MUL: \n");
            show_node_with_indent(node->lhs, level + 1);
            show_node_with_indent(node->rhs, level + 1);
            break;
        case ND_DIV:
            put_indent(level); printf("ND_DIV: \n");
            show_node_with_indent(node->lhs, level + 1);
            show_node_with_indent(node->rhs, level + 1);
            break;
        case ND_IDENT: 
            put_indent(level); printf("ND_IDENT: '%.*s'\n", node->len, node->str);
            break;
        case ND_STRUCT_DECLARATION:
            put_indent(level); printf("ND_STRUCT_DECLARATION\n");
            break;
        // case ND_TYPE_SPECIFIER:
        //     put_indent(level); printf("ND_TYPE_SPECIFIER: '%.*s'\n", node->len, node->str);
        //     if (node->next) {
        //         show_node_with_indent(node->next, level);
        //     }
        //     break;
        case ND_FUNCTION_DEFINITION: 
            put_indent(level); printf("ND_FUNCTION_DEFINITION: \n");
            put_indent(level + 1); show_type(node->share.function_difinition.decl->decl); printf("\n");
            show_node_with_indent(node->share.function_difinition.cs, level + 1);
            break;
        case ND_DECLARATION:
            put_indent(level); printf("ND_DECLARATION: \n");
            for (decl_list_t *cur = node->share.declaration.decls; cur; cur = cur->next) {
                if (cur->self->ident) {
                    put_indent(level + 1); printf("%.*s: ", cur->self->ident->len, cur->self->ident->str);
                } else {
                    PANIC("EXPECTed ident\n");
                }
                show_type(cur->self->decl);
                if (cur->self->init) {
                    put_indent(level + 1); printf("init: \n");
                    show_node_with_indent(cur->self->init, level + 2);
                }
                printf("\n");
            }
            break;
        case ND_COMPOUND_STATEMENT:
            put_indent(level); printf("ND_COMPOUND_STATEMENT: \n");
            for (node_list_t *cur = node->share.compound_statement.block_item_list_opt; cur; cur = cur->next) {
                show_node_with_indent(cur->self, level + 1);
            }
            break;
        case ND_ASSIGNMENT_EXPRESSION:
            put_indent(level); printf("ND_ASSIGNMENT_EXPRESSION: \n");
            show_node_with_indent(node->share.assignment_expression.unary_expression, level + 1);
            put_indent(level + 1); printf("op: %s\n", node->share.assignment_expression.assignment_operator);
            show_node_with_indent(node->share.assignment_expression.rec, level + 1);
            break;
        // case ND_DECLARATOR:
        //     put_indent(level); printf("ND_DECLARATOR: \n");
        //     if (node->share.declarator.ptr_opt) {
        //         show_node_with_indent(node->share.declarator.ptr_opt, level + 1);
        //     }
        //     show_node_with_indent(node->share.declarator.direct_decl, level + 1);
        //     break;
        // case ND_DIRECT_DECLARATOR:
        //     put_indent(level); printf("ND_DIRECT_DECLARATOR:\n");
        //     if (node->share.direct_declarator.ident) {
        //         put_indent(level + 1); printf("Identifier: \n");
        //         show_node_with_indent(node->share.direct_declarator.ident, level + 2);
        //     } else if (node->share.direct_declarator.braced_declarator) {
        //         put_indent(level + 1); printf("Braced: \n");
        //         show_node_with_indent(node->share.direct_declarator.braced_declarator, level + 2);
        //     }

        //     if (node->share.direct_declarator.array) {
        //         put_indent(level + 1); printf("Array: \n");
        //         show_array_with_indent(node->share.direct_declarator.array, level + 2);
        //     }
        //     // TODO: show marr as ini 
        //     break;
        // case ND_INIT_DECLARATOR:
        //     put_indent(level); printf("ND_INIT_DECLARATOR:\n");
        //     show_node_with_indent(node->lhs, level + 1);
        //     if (node->rhs) {
        //         show_node_with_indent(node->rhs, level + 1);
        //     } else {
        //         put_indent(level + 1); printf("initializer: None\n");
        //     }
        //     break;
        // case ND_POINTER:
        //     put_indent(level); printf("ND_POINTER: *\n");
        //     if (node->share.pointer.tqs_opt) {
        //         put_indent(level + 1); printf("type qualifier list optional, but not show yet\n");
        //     } 
        //     if (node->share.pointer.ptr_opt) {
        //         show_node_with_indent(node->share.pointer.ptr_opt, level + 1);
        //     }
        //     break;
        default:
            printf("default");
            break;
    }
}

void show_type(type_t *t) {
    if (!t) {
        printf("show_type: NULL\n");
        exit(1);
    }

    switch (t->kind) {
        case VOID:
            printf("void");
            break;
        case INT:
            printf("int");
            break;
        case PTR:
            printf("* -> (");
            show_type(t->ptr_to);
            printf(")");
            break;
        case ARRAY:
            printf("[] -> (");
            show_type(t->array_to);
            printf(")");
            break;
        case FUN:
            printf("fn(");
            for (decl_list_t *cur = t->params; cur; cur = cur->next) {
                show_type(cur->self->decl);
                if (cur->next)
                    printf(", ");
            }
            printf(") -> (");
            show_type(t->ret);
            printf(")");
            break;
        case STRUCT:
            printf("struct ");
            if (t->share.struct_or_union_specifier.ident_opt) {
                printf("%.*s", t->share.struct_or_union_specifier.ident_opt->len, t->share.struct_or_union_specifier.ident_opt->str);
            }
            break;
        default:
            printf("default\n");
            break;
    }
}

void show_array_with_indent(array_t *array, int level) {
    put_indent(level); printf("[%d]\n", array->len);
    if (array->next) {
        show_array_with_indent(array->next, level + 1);
    }
}

node_t *parse(token_t *token) {
    _cur = token;

    node_t *exd = _external_declaration();
    
    return exd;
}

void consume_brace() {
    EXPECT("(");

    while (_cur) {
        if (!_cur) PANIC("expected )");
        
        if (consume(")")) return; 

        if (peek("(")) consume_brace();
        else _cur = _cur->next;
    }
}

node_t *_external_declaration() {
    node_t *fd = try_(_function_definition);
    if (fd) return fd;

    node_t *decl = _declaration();
    if (decl) return decl;
    
    PANIC("no matching syntax!\n");
}

node_t *_function_definition() {
    printf("trace\n");
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
    if (_cur->kind == TK_IDENT) {
        node_t *ident = new_node(ND_IDENT);
        ident->str = _cur->str;
        ident->len = _cur->len;

        _cur = _cur->next;
        return ident;
    } else {
        return NULL;
    }
}

node_t *constant() {
    if (_cur->kind != TK_NUM) {
        printf("constant(): EXPECT number but\n");
        exit(1);
    }

    node_t *node = (node_t *)calloc(1, sizeof(node_t));
    node->kind = ND_NUM;
    node->val = _cur->val;
    
    _cur = _cur->next;

    return node;
}

node_t *primary_expression() {
    node_t *ident = identifier();
    if (ident) return ident;

    if (_cur->kind == TK_NUM) {
        return constant();
    } else if (consume("(")) {
        node_t *e = expression();
        EXPECT(")");
        return e;
    } else {
        PANIC("primary_expression: unimplemented! %s\n", _cur->str);
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
    token_t *_s = _cur;
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
    token_t *backtrack = _cur;

    type_t *ptr_opt = _pointer(base);
    decl_t *direct_decl = _direct_declarator(ptr_opt);

    if (!direct_decl) {
        _cur = backtrack;
        return NULL;
    }

    return direct_decl;
}

decl_t *_direct_declarator(type_t *base) {
    token_t *b = _cur;
    type_t *braced = NULL;

    node_t *ident = identifier();

    printf("ident\n");
    if (!ident) {
        printf("no %s\n", _cur->str);
        if (consume("(")) {
            decl_t *dectr = _declarator(NULL);
            if (!dectr) {
                _cur = b;
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

    return tspecs;
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
    if (!_cur) PANIC("current token is null\n");

    if (_cur->kind == TK_RESERVED || _cur->kind == TK_IDENT) {
        // printf("compare %s to %.*s\n", op, _cur->len, _cur->str);
        if (_cur->len == strlen(op) && strncmp(_cur->str, op, _cur->len) == 0) {
            // printf("match\n");
            _cur = _cur->next;
            return true;
        }
    }

    return false;
}

bool peek(char *op) {
    if (!_cur) PANIC("current token is null\n");

    if (_cur->kind == TK_RESERVED || _cur->kind == TK_IDENT) {
        if (_cur->len == strlen(op) && strncmp(_cur->str, op, _cur->len) == 0) {
            return true;
        }
    }

    return false;
}

bool type(char *c) {
    if (!_cur) PANIC("current token is null\n");

    // printf("compare %s to %s\n", c, _cur->str);

    if (_cur->kind == TK_TYPE) {
        if (_cur->len == strlen(c) && strncmp(_cur->str, c, _cur->len) == 0) {
            _cur = _cur->next;
            return true;
        }
    }

    return false;
}

void expect(char *op) {
    if (_cur->kind == TK_RESERVED || _cur->kind == TK_IDENT) {
        if (_cur->len == strlen(op) && strncmp(_cur->str, op, _cur->len) == 0) {
            _cur = _cur->next;
            return;
        }
    }

    printf("EXPECT '%s' but '%.*s'\n", op, _cur->len, _cur->str);
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
    if (_cur->kind == TK_TYPE) {
        if (_cur->len == strlen(name) && strncmp(_cur->str, name, _cur->len) == 0) {   
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
    token_t *_backtrack = _cur;
    node_t *res = p();

    if (res) {
        return res;
    } else {
        // printf("try fial\n");
        _cur = _backtrack;
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