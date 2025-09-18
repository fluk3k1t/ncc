#include <stdio.h>
#include <tokenizer.h>
#include <parser.h>

int main(int argc, char *argv[]) {
    char *code = "*[*]";
    
    token_t *token = tokenize(code);
    // show_token(token);

    // node_t *node = parse(token);
    // show_node(node);

    type_t *t = parse_type(token);
    show_type(t);

    return 0;
}