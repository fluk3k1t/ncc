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
        PANIC("node is NULL\n");
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
            show_type(node->share.struct_declaration.struct_declarator_list_opt->self->decl);
            printf("\n");
            break;
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
                    PANIC("expected ident\n");
                }
                show_type(cur->self->decl);
                if (cur->self->init) {
                    put_indent(level + 1); printf("init: \n");
                    show_node_with_indent(cur->self->init, level + 2);
                }
                printf("\n");
            }
            if (node->share.declaration.struct_declaration) {
                put_indent(level + 1); show_type(node->share.declaration.struct_declaration);
                printf("\n");
            }
            break;
        case ND_COMPOUND_STATEMENT:
            put_indent(level); printf("ND_COMPOUND_STATEMENT: \n");
            show_node_with_indent(node->share.compound_statement.block_item_list_opt, level + 1);
            break;
        case ND_ASSIGNMENT_EXPRESSION:
            put_indent(level); printf("ND_ASSIGNMENT_EXPRESSION: \n");
            show_node_with_indent(node->share.assignment_expression.unary_expression, level + 1);
            put_indent(level + 1); printf("op: %s\n", node->share.assignment_expression.assignment_operator);
            show_node_with_indent(node->share.assignment_expression.rec, level + 1);
            break;
        case ND_BLOCK_ITEM_LIST:
            printf("BLOCK_ITEM_LIST: \n");
            for (node_list_t *cur = node->share.block_item_list.list; cur; cur = cur->next) {
                show_node_with_indent(cur->self, level + 1);
            }
            break;
        case ND_EXPRESSION_STATEMENT:
            printf("ND_EXPRESSION_STATEMENT\n");
            if (node->share.expression_statement.expression_opt)
                show_node_with_indent(node->share.expression_statement.expression_opt, level + 1);
            break;
        default:
            printf("default %d\n", node->kind);
            break;
    }
}

void show_type(type_t *t) {
    if (!t) {
        PANIC("show_type: NULL\n");
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
                printf("%.*s\n", t->share.struct_or_union_specifier.ident_opt->len, t->share.struct_or_union_specifier.ident_opt->str);
            }
            show_node(t->share.struct_or_union_specifier.struct_declaration_list);
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
