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

typedef struct {
    decl_t *decl;
    int size;
    int allign;
} env_t;

typedef struct {
    env_t *self[255];
    int depth, allign;
} env_stack_t;

void codegen(const char* output_path, List(ref(node_t)) *nodes);
void _codegen(node_t *node);
void gen_lvar(node_t *node);
env_t *find(char *ident, int len);

#endif