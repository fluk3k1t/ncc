#include "../inc/codegen.h"

#include <stdio.h>
#include <string.h>
#include "../inc/parser.h"

FILE *fp;

env_stack_t env_stack[255];
int env_stack_depth = 0;

void codegen(const char* output_path, node_list_t *nodes) {
    fp = fopen(output_path, "w");
    if (!fp) PANIC("cannot open file '%s'", output_path);

    for (node_list_t *cur = nodes; cur; cur = cur->next) {
        _codegen(cur->self);
    }

    fclose(fp);
    fp = NULL;

    env_stack_t cur_stack = env_stack[env_stack_depth];
    for (int i = 0; i < cur_stack.depth; i++) {
        // printf("lookup: %.*s\n", cur_stack.self[i]->ident->len, cur_stack.self[i]->ident->str);
    }

    printf("%d\n", cur_stack.depth);
}

void _codegen(node_t *node) {
    switch (node->kind) {
        case ND_NUM:
            fprintf(fp, "   push %d\n", node->val);
            break;
        case ND_FUNCTION_DEFINITION:
            TRACE("hello");
            fprintf(fp, "%.*s:\n", node->share.function_difinition.decl->ident->len, node->share.function_difinition.decl->ident->str);
            _codegen(node->share.function_difinition.cs);
            fprintf(fp, "\tret\n");
            break;
        case ND_COMPOUND_STATEMENT:
            TRACE("compound statement\n");
            _codegen(node->share.compound_statement.block_item_list_opt);
            break;
        case ND_BLOCK_ITEM_LIST:
            TRACE("block item list\n");
            for (node_list_t *cur = node->share.block_item_list.list; cur; cur = cur->next) {
                _codegen(cur->self);
            }
            break;
        case ND_BLOCK:
            TRACE("block\n");
            break;
        case ND_DECLARATION:
            TRACE("decl\n");
            // node->share.declaration.
            for (decl_list_t *cur = node->share.declaration.decls; cur; cur = cur->next) {
                TRACE("push\n");
                if (cur->self->decl->share.struct_or_union_specifier.is_definition) {
                    TRACE("is difinition\n");
                    
                } else {
                    TRACE("is not difni\n");
                }
                env_stack_t *cur_stack = &env_stack[env_stack_depth];
                env_t *env = (env_t *)calloc(1, sizeof(env_t));
                env->decl = cur->self;
                env->allign = cur_stack->allign++;
                env->size = 0;
                cur_stack->self[cur_stack->depth++] = env;
            }

            // if (node->share.declaration.struct_declaration) {
                
            // }
            // if (node->share.struct_declaration) {
            //     TRACE("struct decl\n");
            // }
            break;
        case ND_EXPRESSION_STATEMENT:
            if (node->share.expression_statement.expression_opt)
                _codegen(node->share.expression_statement.expression_opt);
            break;
        case ND_ASSIGNMENT_EXPRESSION:
            TRACE("assignment expression\n");
            gen_lvar(node->share.assignment_expression.unary_expression);
            _codegen(node->share.assignment_expression.rec);
            fprintf(fp, "   pop rdi\n");
            fprintf(fp, "   pop rax\n");
            fprintf(fp, "   mov [rax], rdi\n");
            break;
        default:
            printf("default\n");
            fprintf(fp, "default\n");
            break;
    }
}

void gen_lvar(node_t *node) {
    switch (node->kind) {
        case ND_IDENT:
            env_t *lhs = find(node->str, node->len);
            if (!lhs) PANIC("variable '%.*s' is not declared\n", node->len, node->str);
            fprintf(fp, "   mov rax, rbp\n");
            fprintf(fp, "   sub rax, %d\n", lhs->allign);
            fprintf(fp, "   push rax\n");
            break;
        default:
            PANIC("this node can not be treated as lvar!\n");
    }
}

env_t *find(char *ident, int len) {
    env_stack_t *cur_stack = &env_stack[env_stack_depth];
    for (int depth = 0; depth < cur_stack->depth; depth ++) {
        if (strncmp(ident, cur_stack->self[depth]->decl->ident->str, len) == 0) {
            return cur_stack->self[depth];
        }
    } 

    return NULL;
}