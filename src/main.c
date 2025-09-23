#include <stdio.h>
#include "../inc/tokenizer.h"
#include "../inc/parser.h"

int main(int argc, char *argv[]) {
    char *code = "int a = (b)1)";
    
    token_t *token = tokenize(code);
    // show_token(token);

    node_t *node = parse(token);
    show_node(node);
    
    return 0;
}