#include <parser.h>

#include <stdio.h>
#include <tokenizer.h>

token_t *cur = NULL;

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
        default:
            printf("default");
            break;
    }
}


void show_type(type_t *type) {
    show_type_with_indent(type, 0);
    printf("\n");
}

void show_type_with_indent(type_t *type, int level) {
    if (type == NULL) {
        printf("show_type_with_indent: type is NULL\n");
        // exit(1);
        return;
    }

    switch (type->kind) {
        case VOID:
            printf("void");
            break;
        case CHAR:
            printf("char");
            break;
        case SHORT:
            printf("short");
            break;
        case INT:
            printf("int");
            break;
        case LONG:
            printf("long");
            break;
        case PTR:
            printf("%.*s", type->len, type->name);
            printf("* -> ");
            show_type_with_indent(type->ptr, level);
            break;
        case ARRAY:
            printf("array:\n");
            show_type_with_indent(type->ptr, level+1);
            break;
        default:
            printf("default\n");
            break;
    }
}

node_t *parse(token_t *token) {
    cur = token;

    node_t *e = expression();
    
    return e;
}

type_t *new_type(type_kind_t kind) {
    type_t *type = (type_t *)calloc(1, sizeof(type_t));
    type->kind = kind;
    return type;
}

type_t *parse_type(token_t *token) {
    cur = token;

    type_t *int_ = (type_t *)calloc(1, sizeof(type_t));
    int_->kind = INT;
    type_t *t = abstract_decrarator(int_);

    return t;
}

token_t *rollback = NULL;

type_t *declaration_specifiers() {

}

type_t *pointer(type_t *base) {
    if (consume("*")) {
        type_t *ptr = (type_t *)calloc(1, sizeof(type_t));
        ptr->kind = PTR;
        ptr->ptr = base;
        return ptr;
    } else {
        return base;
    }
}

type_t *type_name() {

}

type_t *abstract_decrarator(type_t *base) {
    rollback = cur;

    //
    type_t *ptr_opt = pointer(base);
    type_t *opt = direct_abstract_declarator(ptr_opt);
    if (opt) {
        return opt;
    }

    cur = rollback;

    //
    expect("*");

    type_t *type = (type_t *)calloc(1, sizeof(type_t));
    type->kind = PTR;
    type->ptr = base;
    return type;
}

type_t *direct_abstract_declaration_partial(type_t *base) {
    if (peek("[")) {
        type_t *head = base;

        while (1) {

            consume("[");
            if (peek("*")) {
                consume("*");
                expect("]");

                type_t *array = new_type(ARRAY);
                array->ptr = head;
                head = array;
            } else {
                break;
            }
        }

        return head;
    } else if (peek("(")) {

    } 
}

type_t *direct_abstract_declarator(type_t *base) {
    if (consume("(")) {
        printf("direct\n");
        type_t *ad = abstract_decrarator(NULL);
        expect(")");

        if (ad->kind == PTR) {
            type_t *t = direct_abstract_declaration_partial(base);
            ad->ptr = t;
            return ad;
        } else {
            printf("1unreachable!\n");
            exit(1);
        }
    } else if (peek("[")) {
        return direct_abstract_declaration_partial(base);
    } else {
        printf("3unreachable! %s\n", cur->str);
        // exit(1);
        return NULL;
    }
}

node_t *type_specifier() {
    // node_t *ts = (node_t *)calloc(1, sizeof(node_t));
    // ts->kind = ND_TYPE_SPECIFIER;

    // if (type("int")) {
    //     return ts;
    // }
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
            cur = new_node(ND_MUL, cur, primary_expression());
        } else if (consume("/")) {
            cur = new_node(ND_DIV, cur, primary_expression());
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
            cur = new_node(ND_ADD, cur, multiplicative_expression());
        } else if (consume("-")) {
            cur = new_node(ND_SUB, cur, multiplicative_expression());
        } else {
            break;
        }
    }

    return cur;
}

node_t *expression() {
    node_t *e = additive_expression();
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

bool type(char *name) {
    if (cur->kind == TK_TYPE) {
        if (cur->len == strlen(name) && strncmp(cur->str, name, cur->len) == 0) {
            cur = cur->next;
            return true;
        }
    }
    
    return false;
}

node_t *new_node(node_kind_t kind, node_t *lhs, node_t *rhs) {
    node_t *node = (node_t *)calloc(1, sizeof(node_t));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;

    return node;
}