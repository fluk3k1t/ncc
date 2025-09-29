#include "../inc/semantics.h"
#include "../inc/parser.h"
#include "../inc/pretty.h"

decl_list_t *env = NULL, **tail = &env;

void analyze(node_list_t *ns) {
    for (node_list_t *n = ns; n; n = n->next)
        _analyze(n->self);
}

void _analyze(node_t *node) {
    switch (node->kind) {
        case ND_DECLARATION:
            for (decl_list_t *cur = node->share.declaration.decls; cur; cur = cur->next) {
                decl_list_t *next = (decl_list_t *)calloc(1, sizeof(decl_list_t));
                next->self = cur->self;
                *tail = next;
                tail = &next->next;
            }
            break;
        default:
            printf("analyze: default\n");
            break;
    }
}