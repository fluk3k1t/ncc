#include <stdio.h>
#include "../inc/tokenizer.h"
#include "../inc/parser.h"
#include "../inc/pretty.h"
#include "../inc/semantics.h"
#include "../inc/codegen.h"

int main(int argc, char *argv[]) {
    char *code = "struct token_t {int val; }; ";
    
    token_t *token = tokenize(code);
    // show_token(token);

    node_list_t *ns = parse(token);
    // __list_int__ a;
    LIST(int) l = (LIST(int))calloc(1, sizeof(LIST_TYPE(int)));
    LIST_PUSH(int, l, 1);
    LIST_PUSH(int, l, 2);
    LIST_FOREACH(int, l, itr) {
        printf("f %d\n", itr->inner);
    }
    int a = LIST_POP_FRONT(int, l);
    LIST_FOREACH(int, l, itr) {
        printf("f %d\n", itr->inner);
    }    
    
    int b = LIST_POP(int, l);
    LIST_FOREACH(int, l, itr) {
        printf("f %d\n", itr->inner);
    }    
    int c = LIST_POP(int, l);
    // LIST_FOREACH(int, l, itr) {
    //     printf("f %d\n", itr->inner);
    // }
    // PUSH(a);

    // for (node_list_t *n = ns; n; n = n->next) {
    //     show_node(n->self);
    // }
    // analyze(ns);

    codegen("out.asm", ns);
    
    return 0;
}