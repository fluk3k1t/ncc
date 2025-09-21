#ifndef __H_SEMANTICS
#define __H_SEMANTICS

#include <parser.h>

typedef enum type_kind_t type_kind_t;
enum type_kind_t {
    VOID, 
    CHAR, 
    SHORT, 
    INT, 
    LONG, 
    STRUCT, 
    PTR, 
    ARRAY, 
    FUN,
};

typedef struct type_t type_t;
struct type_t {
    type_kind_t kind;
    type_t *ptr_to;
    char *str;
    int len;
};

type_t *type_of(node_t *typename_without_ident);
void show_type(type_t *t);

#endif