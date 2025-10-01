#include <stdio.h>
#include "../inc/tokenizer.h"
#include "../inc/parser.h"
#include "../inc/pretty.h"
#include "../inc/semantics.h"
#include "../inc/codegen.h"
#include "../inc/list.h"

int main(int argc, char *argv[]) {
    char *code = "const volatile";
    
    token_t *token = tokenize(code);
    List(ref(Node)) *tree = parse(token);

    return 0;
}