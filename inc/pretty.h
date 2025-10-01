#ifndef __H_PRETTY_
#define __H_PRETTY_

#include "tokenizer.h"
#include "parser.h"

#define PANIC(fmt, ...)                                                     \
    do {                                                                    \
        fprintf(stderr,                                                     \
                "PANIC at %s:%d in %s(): " fmt " rest: %.*s\n",             \
                __FILE__, __LINE__, __func__, ##__VA_ARGS__, __cur->len, __cur->str);\
        fflush(stderr);                                                     \
        exit(1);                                                            \
    } while (0)

void show_node(node_t *node);
void show_node_with_indent(node_t *node, int level);
// void show_array_with_indent(array_t *array, int level);
void _show_type(type_t *type);

void show_type(Type *type);

#endif