#include "../inc/pretty.h"

extern token_t *_cur;

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
