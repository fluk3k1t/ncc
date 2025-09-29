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

    // for (node_list_t *n = ns; n; n = n->next) {
    //     show_node(n->self);
    // }
    // analyze(ns);

    codegen("out.asm", ns);
    
    return 0;
}