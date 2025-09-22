#include <parser.h>

#include <stdio.h>
#include <tokenizer.h>

static token_t *cur = NULL;
static node_t *backtrack = NULL;

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
        case ND_TYPE_SPECIFIER:
            put_indent(level); printf("ND_TYPE_SPECIFIER: '%.*s'\n", node->len, node->str);
            if (node->next) {
                show_node_with_indent(node->next, level);
            }
            break;
        case ND_DECLARATION:
            put_indent(level); printf("ND_DECLARATION: \n");
            show_node_with_indent(node->share.declaration.decl_specs, level + 1);
            if (node->share.declaration.init_decl_list_opt) {
                show_node_with_indent(node->share.declaration.init_decl_list_opt, level + 1);
            }
            break;
        case ND_DECLARATOR:
            put_indent(level); printf("ND_DECLARATOR: \n");
            if (node->share.declarator.ptr_opt) {
                show_node_with_indent(node->share.declarator.ptr_opt, level + 1);
            }
            show_node_with_indent(node->share.declarator.direct_decl, level + 1);
            break;
        case ND_DIRECT_DECLARATOR:
            put_indent(level); printf("ND_DIRECT_DECLARATOR:\n");
            if (node->share.direct_declarator.ident) {
                put_indent(level + 1); printf("Identifier: \n");
                show_node_with_indent(node->share.direct_declarator.ident, level + 2);
            } else if (node->share.direct_declarator.braced_declarator) {
                put_indent(level + 1); printf("Braced: \n");
                show_node_with_indent(node->share.direct_declarator.braced_declarator, level + 2);
            }

            if (node->share.direct_declarator.array) {
                put_indent(level + 1); printf("Array: \n");
                show_array_with_indent(node->share.direct_declarator.array, level + 2);
            }
            // TODO: show marr as ini 
            break;
        case ND_INIT_DECLARATOR:
            put_indent(level); printf("ND_INIT_DECLARATOR:\n");
            show_node_with_indent(node->lhs, level + 1);
            if (node->rhs) {
                show_node_with_indent(node->rhs, level + 1);
            } else {
                put_indent(level + 1); printf("initializer: None\n");
            }
            break;
        case ND_POINTER:
            put_indent(level); printf("ND_POINTER: *\n");
            if (node->share.pointer.tqs_opt) {
                put_indent(level + 1); printf("type qualifier list optional, but not show yet\n");
            } 
            if (node->share.pointer.ptr_opt) {
                show_node_with_indent(node->share.pointer.ptr_opt, level + 1);
            }
            break;
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
            printf("VOID\n");
            break;
        case INT:
            printf("INT\n");
            break;
        case PTR:
            printf("PTR\n");
            show_type(t->ptr_to);
            break;
        case ARRAY:
            printf("ARRAY\n");
            show_type(t->array_to);
            break;
        case FUN:
            printf("FUN\n");
            show_type(t->ret);
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

type_t *_declaration() {
    type_t *decl_specs = _declaration_specifiers();
    decl_t *init_dectr = _init_declarator(decl_specs);

    // return init_dectr;
    show_type(init_dectr->decl);

    return NULL;
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

decl_t *_init_declarator(type_t *base) {
    decl_t *dectr = _declarator(base);
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
            braced = _declarator(NULL)->decl;
            expect(")");
        } else {
            PANIC("unimplemented\n");
        }
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
            if (tail->ptr_to || tail->array_to) {
                if (tail->kind == PTR) {
                    tail = tail->ptr_to;
                } else if (tail->kind == ARRAY) {
                    tail = tail->array_to;
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
        } else {
            PANIC("unreachable");
        }

        base = braced;
    }

    // decl_tはただのtype_tとidentのタプルなので動的確保しなくてもいいが混在するとややこしいので、、、
    decl_t *d = (decl_t *)calloc(1, sizeof(decl_t));
    d->decl = base;
    d->ident = ident;

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

// (6.7)
node_t *declaration_specifiers() {
    node_t *cur = (node_t *)calloc(1, sizeof(node_t));
    node_t *head = cur;

    while (1) {
        node_t *type_spec = type_specifier();
        if (type_spec) {
            cur->next = type_spec;
            cur = type_spec;
            continue;
        }

        break;
    }

    return head->next;
}

// (6.7)
node_t *init_declarator_list() {
    return init_declarator();
}

// (6.7)
node_t *init_declarator() {
    node_t *decl = declarator();
    if (!decl) return NULL;

    node_t *init_decl = new_node_with(ND_INIT_DECLARATOR, decl, NULL);

    if (consume("=")) {
        node_t *init = initializer();
        init_decl->rhs = init;
        return init_decl;
    } else {
        return init_decl;
    }
}

extern const char *types[];
extern const unsigned int types_len;

// (6.7.2)
node_t *type_specifier() {
    char *type_spec_maybe = peek_types(types, types_len);
    if (type_spec_maybe) {
        node_t *type_spec = new_node(ND_TYPE_SPECIFIER);
        type_spec->str = cur->str;
        type_spec->len = cur->len;
        cur = cur->next;
        return type_spec;
    } else {
        return NULL;
    }
}

// (6.7.3)
node_t *type_qualifier() {
    if (consume("const")) {
        node_t *tq = new_node(ND_TYPE_QUALIFIER);
        tq->str = cur->str;
        tq->len = cur->len;
        return tq;
    }

    return NULL;
}

// (6.7.6)
node_t *declarator() {
    node_t *ptr_opt = pointer();
    node_t *direct_decl = direct_declarator();

    if (direct_decl) {
        node_t *decl = new_node(ND_DECLARATOR);
        decl->share.declarator.ptr_opt = ptr_opt;
        decl->share.declarator.direct_decl = direct_decl;
        return decl;
    } else {
        printf("declarator(): expected 'direct_declarator'\n");
        exit(1);
    }
}

// (6.7.6)
node_t *direct_declarator() {
    node_t *direct_decl = new_node(ND_DIRECT_DECLARATOR);

    node_t *ident = identifier();
    if (ident) {
        direct_decl->share.direct_declarator.ident = ident;   
    } else {
        expect("(");
        node_t *braced_declarator = declarator();
        if (!braced_declarator) {
        printf("expected declarator\n");
            exit(1);
        }
        expect(")");
        direct_decl->share.direct_declarator.braced_declarator = braced_declarator;
    }

    array_t *arr = (array_t *)calloc(1, sizeof(array_t));
    array_t *head = arr;

    for (int i = 0; ; i++) {
        if (consume("[")) {
            expect("]");
            array_t *new_arr = (array_t *)calloc(1, sizeof(array_t));
            new_arr->len = 46;
            arr->next = new_arr;
            arr = new_arr;
            continue;
        } else if (consume("(")) {
            expect(")");
            continue;
        }

        break;
    }

    direct_decl->share.direct_declarator.array = head->next;

    return direct_decl;
}

// (6.7.6)
node_t *pointer() {
    node_t *ptrs = new_node(ND_POINTER);
    node_t *head = ptrs;

    while (1) {
        if (consume("*")) {
            node_t *tqs_opt = type_qualifier_list();
            node_t *ptr = new_node(ND_POINTER);
            ptr->share.pointer.tqs_opt = tqs_opt;
            ptrs->share.pointer.ptr_opt = ptr;
            ptrs = ptr;
            continue;
        }

        break;
    }

    return head->share.pointer.ptr_opt;
}

// (6.7.6)
node_t *type_qualifier_list() {
    node_t *tqs = new_node(ND_TYPE_QUALIFIER);
    node_t *head = tqs;

    while (1) {
        node_t *tq = type_qualifier();
        if (tq) {
            tqs->next = tq;
            tqs = tq;
            continue;
        }

        break;
    }

    return head->next;
}

// (6.7.9)
node_t *initializer() {
    // tmp
    return expression();
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