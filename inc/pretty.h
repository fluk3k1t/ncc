#ifndef __H_PRETTY_
#define __H_PRETTY_

#include "tokenizer.h"
#include "parser.h"

void show_node(node_t *node);
void show_node_with_indent(node_t *node, int level);
void show_array_with_indent(array_t *array, int level);
void show_type(type_t *type);

#endif