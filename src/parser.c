#include <parser.h>

#include <stdio.h>
#include <tokenizer.h>

static token_t *cur = NULL;
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
        // case ND_TYPE_SPECIFIER:
        //     put_indent(level); printf("ND_TYPE_SPECIFIER: '%.*s'\n", node->len, node->str);
        //     if (node->next) {
        //         show_node_with_indent(node->next, level);
        //     }
        //     break;
        case ND_DECLARATION:
            put_indent(level); printf("ND_DECLARATION: \n");
            for (decl_list_t *cur = node->share.declaration.decls; cur; cur = cur->next) {
                if (cur->self->ident) {
                    put_indent(level + 1); printf("%.*s: ", cur->self->ident->len, cur->self->ident->str);
                } else {
                    PANIC("expected ident\n");
                }
                show_type(cur->self->decl);
                printf("\n");
            }
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
    cur = token;

    type_t *e = _declaration();
    
    return e;
}

node_t *identifier() {
    if (cur->kind == TK_IDENT) {
        node_t *ident = new_node(ND_IDENT);
        ident->str = cur->str;
        ident->len = cur->len;

        cur = cur->next;
        return ident;
    } else {
        return NULL;
    }
}

node_t *constant() {
    if (cur->kind != TK_NUM) {
        printf("constant(): expect number but\n");
        exit(1);
    }

    node_t *node = (node_t *)calloc(1, sizeof(node_t));
    node->kind = ND_NUM;
    node->val = cur->val;
    
    cur = cur->next;

    return node;
}

node_t *primary_expression() {
    if (cur->kind == TK_NUM) {
        return constant();
    } else if (consume("(")) {
        node_t *e = expression();
        expect(")");
        return e;
    } else {
        printf("primary_expression: unimplemented!\n");
        exit(1);
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

node_t *expression() {
    node_t *e = additive_expression();
}

node_t *_declaration() {
    type_t *decl_specs = _declaration_specifiers();
    if (!decl_specs) return NULL;

    decl_list_t *init_dectr_list = _init_declarator_list(decl_specs);
    if (!init_dectr_list) { PANIC("expected '_init_declarator_list'\n"); }

    node_t *decl = new_node(ND_DECLARATION);
    decl->share.declaration.decls = init_dectr_list;

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
        if (!i) { PANIC("expected init-declarator"); }
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
        if (!init) { PANIC("expected initializer"); }
        dectr->init = init;
    }

    return dectr;
}

type_t *_type_specifier() {
    if (strncmp(cur->str, "int", 3) == 0) {
        type_t *t = (type_t *)calloc(1, sizeof(type_t));
        t->kind = INT;
        cur = cur->next;
        return t;
    }

    return NULL;
}

decl_t *_declarator(type_t *base) {
    type_t *ptr_opt = _pointer(base);
    decl_t *direct_decl = _direct_declarator(ptr_opt);

    return direct_decl;
}

decl_t *_direct_declarator(type_t *base) {
    type_t *braced = NULL;

    node_t *ident = identifier();

    if (!ident) {
        if (consume("(")) {
            decl_t *dectr = _declarator(NULL);
            braced = dectr->decl;
            // 冗長かも?
            ident = dectr->ident;
            expect(")");
        } else {
            PANIC("unimplemented %s\n", cur->str);
        }
    }

    while (1) {
        if (consume("[")) {
            expect("]");
            type_t *arr = new_type(ARRAY);
            arr->array_to = base;
            base = arr;
        } else if (consume("(")) {
            decl_list_t *ps = _parameter_list();
            expect(")");
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
    decl_list_t *cur = (decl_list_t *)calloc(1, sizeof(decl_list_t));
    decl_list_t *head = cur;

    decl_t *p = _parameter_declaration();
    if (p) {
        do {
            decl_list_t *next = (decl_list_t *)calloc(1, sizeof(decl_list_t));
            next->self = p;
            cur->next = next;
            cur = next;
        } while (consume(","));
    }

    return head->next;
}

decl_t *_parameter_declaration() {
    type_t *tspecs = _declaration_specifiers();
    if (!tspecs) return NULL;

    decl_t *abst_dectr = _abstract_declarator(tspecs);
    if (abst_dectr) return abst_dectr;

    decl_t *dectr = _declarator(tspecs);
    if (!dectr) { PANIC("expected declarator()"); }

    return dectr;

    // decl_t *dectr = _declarator(tspecs);
    // if (dectr) return dectr;

    // decl_t *abst_dectr = _abstract_declarator(tspecs);
    // if (!abst_dectr) { PANIC("expected abstract-declarator()"); }

    PANIC("panic");

    // return abst_dectr;
}

decl_t *_abstract_declarator(type_t *base) {
    type_t *ptr = _pointer(base);
    decl_t *direct_abst_dectr = _direct_abstract_declarator(ptr);
    
    if (!ptr) {
        if (!direct_abst_dectr) { PANIC("expected direct-abstract-declarator()"); }
    } else {
        if (!direct_abst_dectr) return ptr;
    }

    return direct_abst_dectr;
}

decl_t *_direct_abstract_declarator(type_t *base) {
    type_t *braced = NULL;

    if (consume("(")) {
        // TODO: つまりdecl_t ではなくtype_tで事足りる
        braced = _abstract_declarator(NULL)->decl;
        expect(")");
    }

    while (1) {
        if (consume("[")) {
            expect("]");
            type_t *arr = new_type(ARRAY);
            arr->array_to = base;
            base = arr;
        } else if (consume("(")) {
            expect(")");
            type_t *fn = new_type(FUN);
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

// type_t *_direct_abstract_declarator_partial(type_t *base) {
//     while (1) {
//         if (consume("[")) {
//             expect("]");
//             type_t *arr = new_type(ARRAY);
//             arr->array_to = base;
//             base = arr;
//         } else if (consume("(")) {
//             expect(")");
//             type_t *fn = new_type(FUN);
//             fn->ret = base;
//             base = fn;
//         } else {
//             break;
//         }
//     }

//     return base;
// }

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
    return expression();
}

node_t *_function_difinition() {
    type_t *tspecs = _declaration_specifiers();
    if (!tspecs) return NULL;

    decl_t *dectr = _declarator(tspecs);
    if (!dectr) { PANIC("expected '_declarator()'\n"); }

    node_t *fd = new_node(ND_FUNCTION_DEFINITION);
    fd->share.function_difinition.decl = dectr;

    return fd;
}

bool consume(char *op) {
    if (cur->kind == TK_RESERVED || cur->kind == TK_IDENT) {
        if (cur->len == strlen(op) && strncmp(cur->str, op, cur->len) == 0) {
            cur = cur->next;
            return true;
        }
    }

    return false;
}

bool peek(char *op) {
    if (cur->kind == TK_RESERVED || cur->kind == TK_IDENT) {
        if (cur->len == strlen(op) && strncmp(cur->str, op, cur->len) == 0) {
            return true;
        }
    }

    return false;
}

void expect(char *op) {
    if (cur->kind == TK_RESERVED || cur->kind == TK_IDENT) {
        if (cur->len == strlen(op) && strncmp(cur->str, op, cur->len) == 0) {
            cur = cur->next;
            return;
        }
    }

    printf("expect '%s' but '%.*s'\n", op, cur->len, cur->str);
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
    if (cur->kind == TK_TYPE) {
        if (cur->len == strlen(name) && strncmp(cur->str, name, cur->len) == 0) {   
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
    backtrack = cur;
    node_t *res = p();
    if (res) {
        return res;
    } else {
        printf("try fial\n");
        cur = backtrack;
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