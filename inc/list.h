#ifndef __LIST_H_
#define __LIST_H_

#include <stdio.h>
#include <stdlib.h>

#define LIST_ENTRY(T)    \
    _LIST_ENTRY(T)

#define _LIST_ENTRY(T)  \
    list_entry_##T

#define LIST_CONTAINER(T)   \
    _LIST_CONTAINER(T)
    
#define _LIST_CONTAINER(T)  \
    list_container_##T

#define F(T, f) _F(T, f)

#define _F(T, f) T##_##f

#define _LIST_DECLARE(DECL, ALIAS) \
    typedef struct LIST_ENTRY(ALIAS) LIST_ENTRY(ALIAS);   \
    typedef struct LIST_CONTAINER(ALIAS) LIST_CONTAINER(ALIAS);   \
    struct LIST_ENTRY(ALIAS) { \
        struct LIST_ENTRY(ALIAS) *next;    \
        DECL data; \
    };  \
    struct LIST_CONTAINER(ALIAS) { \
        struct LIST_ENTRY(ALIAS) *head, **tail;   \
        size_t len; \
    };  \
    LIST_CONTAINER(ALIAS) *F(LIST_CONTAINER(ALIAS), new) ();  \
    void F(LIST_CONTAINER(ALIAS), push) (LIST_CONTAINER(ALIAS) *co, DECL data); \
    DECL F(LIST_CONTAINER(ALIAS), pop) (LIST_CONTAINER(ALIAS) *co);

#define _LIST_DEFINE(DECL, ALIAS)  \
    LIST_CONTAINER(ALIAS) *F(LIST_CONTAINER(ALIAS), new) () { \
        LIST_CONTAINER(ALIAS) *co = (LIST_CONTAINER(ALIAS *))calloc(1, sizeof(LIST_CONTAINER(ALIAS)));    \
        co->tail = &co->head;   \
        return co;  \
    }   \
    void F(LIST_CONTAINER(ALIAS), push) (LIST_CONTAINER(ALIAS) *co, DECL data) {   \
        LIST_ENTRY(ALIAS) *en = (LIST_ENTRY(ALIAS *))calloc(1, sizeof(LIST_ENTRY(ALIAS))); \
        en->data = data;    \
        *(co->tail) = en;   \
        co->tail = &en->next;   \
    }   \
    DECL F(LIST_CONTAINER(ALIAS), pop) (LIST_CONTAINER(ALIAS) *co) {    \
        if (!co->head) {    \
            printf("list_container_" #ALIAS "_pop(): empty list!\n");  \
            exit(1);    \
        }   \
        DECL data = co->head->data;    \
        co->head = co->head->next;  \
        return data;    \
    }
    
#define DeriveList(T) _LIST_DECLARE(T)
#define DefineList(T) _LIST_DEFINE(T)

#define List(T) \
    _LIST(T)

#define _LIST(_, ALIAS) \
    LIST_CONTAINER(ALIAS)

#define list_new(T) \
    _LIST_NEW(T)

#define _LIST_NEW(_, ALIAS) \
    F(LIST_CONTAINER(ALIAS), new)()

#define list_foreach(T,  co, itr) \
    _LIST_FOREACH(T, co, itr)    

#define _LIST_FOREACH(_, ALIAS,  co, itr) \
    for (LIST_ENTRY(ALIAS) *itr = co->head; itr; itr = itr->next)

#define list_push(T, co, data)  \
    _LIST_PUSH(T, co, data)

#define _LIST_PUSH(_, ALIAS, co, data)  \
    F(LIST_CONTAINER(ALIAS), push)(co, data)
    
#define list_pop(T, co) \
    _LIST_POP(T, co)

#define _LIST_POP(_, ALIAS, co) \
    F(LIST_CONTAINER(ALIAS), pop)(co)

#define ref(T) T*, T##_ref
#define pure(T) T, T

#endif