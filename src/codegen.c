#include "../inc/codegen.h"

#include <stdio.h>
#include "../inc/parser.h"

FILE *fp;

typedef struct {
    decl_t *self[255];
    int depth;
} env_stack_t;

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
        printf("lookup: %.*s\n", cur_stack.self[i]->ident->len, cur_stack.self[i]->ident->str);
    }
    
    printf("%d\n", cur_stack.depth);
}

void _codegen(node_t *node) {
    switch (node->kind) {
        case ND_FUNCTION_DEFINITION:
            TRACE("hello");
            fprintf(fp, "%.*s:\n", node->share.function_difinition.decl->ident->len, node->share.function_difinition.decl->ident->str);
            fprintf(fp, "\tret\n");
            break;
        case ND_DECLARATION:
            TRACE("decl\n");
            for (decl_list_t *cur = node->share.declaration.decls; cur; cur = cur->next) {
                TRACE("push\n");
                env_stack_t *cur_stack = &env_stack[env_stack_depth];
                cur_stack->self[cur_stack->depth++] = cur->self;
            }
            break;
        default:
            printf("default\n");
            break;
    }
}