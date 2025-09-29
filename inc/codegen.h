#ifndef __H_CODEGEN_
#define __H_CODEGEN_

#include "parser.h"

#define ENABLE_TRACE 1

#ifdef ENABLE_TRACE
    #define TRACE(fmt, ...) \
        do {                \
            printf(fmt, ##__VA_ARGS__);                \
        } while (0);        
#else
    #define TRACE(fmt, ...) \
        do {                \
                            \
        } while (0);        
#endif

void codegen(const char* output_path, node_list_t *nodes);
void _codegen(node_t *node);

#endif