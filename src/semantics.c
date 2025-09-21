#include <semantics.h>

#include <parser.h>

type_kind_t classify_type_specifier(node_t *type_spec) {
    if (type_spec->kind == ND_TYPE_SPECIFIER) {
        if (strncmp(type_spec->str, "void", type_spec->len) == 0) {
            return VOID;
        } else if (strncmp(type_spec->str, "int", type_spec->len) == 0) {
            return INT;
        }

        printf("classify_type_specifier: unreachable\n");
        exit(1);
    }

    printf("classify_type_specifier: unreachable\n");
    exit(1);
}

type_t *type_of(node_t *declaration) {
    node_t *decl_specs = declaration->share.declaration.decl_specs;
    node_t *init_decl_list_opt = declaration->share.declaration.init_decl_list_opt;

    if (init_decl_list_opt) {
        // init_declがリストに対応していないまだ
        node_t *init_decl = init_decl_list_opt;
        node_t *declarator = init_decl->lhs;

        node_t *tail_spec = decl_specs;
        for (; tail_spec->next; tail_spec = tail_spec->next);

        // TODO: 基底型チェック
        if (tail_spec->kind != ND_TYPE_SPECIFIER) {
            printf("type_of: expected type specifier\n");
            exit(1);
        }

        type_kind_t tail_spec_kind = classify_type_specifier(tail_spec);
        type_t *base = (type_t *)calloc(1, sizeof(type_t));
        base->kind = tail_spec_kind;           

        if (declarator->share.declarator.ptr_opt) {
            node_t *tail = declarator->share.declarator.ptr_opt;
            type_t *cur = (type_t *)calloc(1, sizeof(type_t));
            cur->kind = PTR;
            type_t *head = cur;
            for (; tail; tail = tail->share.pointer.ptr_opt) {
                // cur->ptr_to = (type_t *)calloc(1, sizeof(type_t));
                // cur->ptr_to->kind = PTR;
                // cur = cur->ptr_to;
                type_t *ptr = (type_t *)calloc(1, sizeof(type_t));
                ptr->kind = PTR;
                ptr->ptr_to = base;
                base = ptr;
            }

            // cur->ptr_to = base;
            // base = head->ptr_to;
        }

        return base;
    } else {
        printf("type_of: unimplemented\n");
        exit(1);
    }
}

void show_type(type_t *t) {
    if (!t) {
        printf("show_type: NULL\n");
        exit(1);
    }

    switch (t->kind) {
        case VOID:
            printf("VOID\n");
            break;
        case INT:
            printf("INT\n");
            break;
        case PTR:
            printf("PTR\n");
            show_type(t->ptr_to);
            break;
        default:
            printf("default\n");
            break;
    }
}