#ifndef KU_SLICE_H
#define KU_SLICE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "fatal.h"

uint32_t get_new_cap(uint32_t cap);

#define SLICE(type, ...)                                                                                               \
    (slice_of_##type##s) {                                                                                             \
        .is_owner = false, .elem = (type *)(type[])__VA_ARGS__, .len = sizeof((type[])__VA_ARGS__),                    \
        .cap = sizeof((type[])__VA_ARGS__),                                                                            \
    }

#define TYPEDEF_SLICE(type)                                                                                            \
    typedef struct slice_of_##type##s slice_of_##type##s;                                                              \
    struct slice_of_##type##s {                                                                                        \
        bool is_owner;                                                                                                 \
        type *elem;                                                                                                    \
        uint32_t len;                                                                                                  \
        uint32_t cap;                                                                                                  \
    };                                                                                                                 \
    slice_of_##type##s new_null_slice_of_##type##s();                                                                  \
    void append_##type##_to_slice(slice_of_##type##s *s, type x);                                                      \
    void free_slice_of_##type##s(slice_of_##type##s s);

#define IMPLEMENT_SLICE(type)                                                                                          \
    slice_of_##type##s new_null_slice_of_##type##s() {                                                                 \
        slice_of_##type##s s = {                                                                                       \
            .is_owner = false,                                                                                         \
            .elem     = NULL,                                                                                          \
            .len      = 0,                                                                                             \
            .cap      = 0,                                                                                             \
        };                                                                                                             \
        return s;                                                                                                      \
    }                                                                                                                  \
                                                                                                                       \
    bool is_null_slice_of_##type##s(slice_of_##type##s s) {                                                            \
        return s.elem == NULL;                                                                                         \
    }                                                                                                                  \
                                                                                                                       \
    void recap_slice_of_##type##s(slice_of_##type##s *s, uint32_t cap) {                                               \
        uint64_t new_size = ((uint64_t)cap) * ((uint64_t)sizeof(type));                                                \
        s->cap            = cap;                                                                                       \
                                                                                                                       \
        if (s->elem == NULL) {                                                                                         \
            type *new_ptr = (type *)malloc(new_size);                                                                  \
            if (new_ptr == NULL) {                                                                                     \
                fatal(1, "not enough memory to resize a slice");                                                       \
            }                                                                                                          \
            s->is_owner = true;                                                                                        \
            s->elem     = new_ptr;                                                                                     \
            return;                                                                                                    \
        }                                                                                                              \
                                                                                                                       \
        if (s->is_owner) {                                                                                             \
            type *new_ptr = (type *)realloc(s->elem, new_size);                                                        \
            if (new_ptr == NULL) {                                                                                     \
                fatal(1, "not enough memory to resize a slice");                                                       \
            }                                                                                                          \
            s->elem = new_ptr;                                                                                         \
            return;                                                                                                    \
        }                                                                                                              \
                                                                                                                       \
        type *new_ptr = (type *)malloc(new_size);                                                                      \
        if (new_ptr == NULL) {                                                                                         \
            fatal(1, "not enough memory to resize a slice");                                                           \
        }                                                                                                              \
                                                                                                                       \
        s->is_owner = true;                                                                                            \
        memcpy(new_ptr, s->elem, ((uint64_t)s->len) * ((uint64_t)sizeof(type)));                                       \
        s->elem = new_ptr;                                                                                             \
    }                                                                                                                  \
                                                                                                                       \
    void append_##type##_to_slice(slice_of_##type##s *s, type x) {                                                     \
        if (s->len < s->cap) {                                                                                         \
            s->elem[s->len] = x;                                                                                       \
            s->len++;                                                                                                  \
            return;                                                                                                    \
        }                                                                                                              \
                                                                                                                       \
        recap_slice_of_##type##s(s, get_new_cap(s->cap));                                                              \
        s->elem[s->len] = x;                                                                                           \
        s->len++;                                                                                                      \
    }                                                                                                                  \
                                                                                                                       \
    void free_slice_of_##type##s(slice_of_##type##s s) {                                                               \
        if (s.is_owner) {                                                                                              \
            free(s.elem);                                                                                              \
        }                                                                                                              \
    }

#endif // KU_SLICE_H
