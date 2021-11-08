#ifndef KU_SLICE_H
#define KU_SLICE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "fatal.h"

const uint32_t first_grow_threshold  = 1 << 16;
const uint32_t second_grow_threshold = 1 << 20;
const uint32_t third_grow_threshold  = 1 << 24;
const uint32_t forth_grow_threshold  = 1 << 28;

#define SLICE(type, ...)                                                                                               \
    (slice_of_##type##s) {                                                                                             \
        .is_owner = false, .elem = (type *)(type[]){__VA_ARGS__}, .len = sizeof((type[]){__VA_ARGS__}),                \
        .cap = sizeof((type[]){__VA_ARGS__}),                                                                          \
    }

#define TYPEDEF_SLICE(type)                                                                                            \
    typedef struct slice_of_##type##s slice_of_##type##s;                                                              \
    struct slice_of_##type##s {                                                                                        \
        bool is_owner;                                                                                                 \
        type *elem;                                                                                                    \
        uint32_t len;                                                                                                  \
        uint32_t cap;                                                                                                  \
    };                                                                                                                 \
    slice_of_ints new_null_slice_of_##type##s();                                                                       \
    void append_##type##_to_slice(slice_of_##type##s *s, type x);                                                      \
    void free_slice_of_##type##s(slice_of_##type##s s);

TYPEDEF_SLICE(int);

slice_of_ints new_null_slice_of_ints() {
    slice_of_ints s = {
        .is_owner = false,
        .elem     = NULL,
        .len      = 0,
        .cap      = 0,
    };
    return s;
}

bool is_null_slice_of_ints(slice_of_ints s) {
    return s.elem == NULL;
}

uint32_t get_new_cap(uint32_t cap) {
    uint32_t new_cap;
    if (cap == 0) {
        new_cap = 1;
    } else if (cap < first_grow_threshold) {
        new_cap = cap << 1;
    } else if (cap < second_grow_threshold) {
        new_cap = cap + (cap >> 1);
    } else if (cap < third_grow_threshold) {
        new_cap = cap + (cap >> 2);
    } else if (cap < forth_grow_threshold) {
        new_cap = cap + (cap >> 3);
    } else {
        new_cap = cap + (cap >> 4);
    }

    return new_cap;
}

void recap_slice_of_ints(slice_of_ints *s, uint32_t cap) {
    uint64_t new_size = ((uint64_t)cap) * ((uint64_t)sizeof(int));
    s->cap            = cap;

    if (s->elem == NULL) {
        int *new_ptr = (int *)malloc(new_size);
        if (new_ptr == NULL) {
            fatal(1, "not enough memory to resize a slice");
        }
        s->is_owner = true;
        s->elem     = new_ptr;
        return;
    }

    if (s->is_owner) {
        int *new_ptr = (int *)realloc(s->elem, new_size);
        if (new_ptr == NULL) {
            fatal(1, "not enough memory to resize a slice");
        }
        s->elem = new_ptr;
        return;
    }

    int *new_ptr = (int *)malloc(new_size);
    if (new_ptr == NULL) {
        fatal(1, "not enough memory to resize a slice");
    }

    s->is_owner = true;
    memcpy(new_ptr, s->elem, ((uint64_t)s->len) * ((uint64_t)sizeof(int)));
    s->elem = new_ptr;
}

void append_int_to_slice(slice_of_ints *s, int x) {
    if (s->len < s->cap) {
        s->elem[s->len] = x;
        s->len++;
        return;
    }

    recap_slice_of_ints(s, get_new_cap(s->cap));
    s->elem[s->len] = x;
    s->len++;
}

void free_slice_of_ints(slice_of_ints s) {
    if (s.is_owner) {
        free(s.elem);
    }
}

#endif // KU_SLICE_H
