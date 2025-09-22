#include <stdio.h>
#include <tokenizer.h>
#include <parser.h>

int main(int argc, char *argv[]) {
    char *code = "int (*f)(int (*)[])";
    
    token_t *token = tokenize(code);
    // show_token(token);

    node_t *node = parse(token);
    show_node(node);
    
    return 0;
}