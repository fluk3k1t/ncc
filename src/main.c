#include <stdio.h>
#include "../inc/tokenizer.h"
#include "../inc/parser.h"
#include "../inc/pretty.h"
#include "../inc/semantics.h"
#include "../inc/codegen.h"
#include "../inc/list.h"

LIST_DECLARE(ref(int))
LIST_DEFINE(ref(int))

int main(int argc, char *argv[]) {
    LIST(ref(int)) *list = LIST_NEW(ref(int));
    int b = 8;
    int *a = &b;
    LIST_PUSH(ref(int), list, a);
    int c = 7;
    int *d = &c;
    LIST_PUSH(ref(int), list, d);
    LIST_FOREACH(ref(int), list, itr) {
        printf("f %d\n", *itr->data);
    }
    int *f = LIST_POP(ref(int), list);
    LIST_FOREACH(ref(int), list, itr) {
        printf("f %d\n", *itr->data);
    }
        int *ff = LIST_POP(ref(int), list);
    LIST_FOREACH(ref(int), list, itr) {
        printf("f %d\n", *itr->data);
    }

    char *code = "struct token_t {int val; }; ";
    
    token_t *token = tokenize(code);
    node_list_t *ns = parse(token);
    // __list_int__ a;
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