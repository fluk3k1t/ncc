#include <stdio.h>
#include <tokenizer.h>
#include <parser.h>
#include <semantics.h>

int main(int argc, char *argv[]) {
    char *code = "int **a;";
    
    token_t *token = tokenize(code);
    // show_token(token);

    node_t *node = parse(token);
    show_node(node);

    type_t *t = type_of(node);
    show_type(t);

    return 0;
}