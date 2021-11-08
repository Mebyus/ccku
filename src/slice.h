#ifndef KU_SLICE_H
#define KU_SLICE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "fatal.h"

typedef struct slice_of_ints slice_of_ints;

struct slice_of_ints {
    bool is_owner;
    int *elem;
    uint32_t len;
    uint32_t cap;
};

slice_of_ints new_null_slice_of_ints();
void insert_int_in_slice(slice_of_ints *s, int x);
void free_slice_of_ints(slice_of_ints s);

slice_of_ints new_null_slice_of_ints() {
    slice_of_ints s = {
        .is_owner = false,
        .elem     = NULL,
        .len      = 0,
        .cap      = 0,
    };
    return s;
}

void grow_slice_of_ints(slice_of_ints *s, uint32_t size) {
    
}

void insert_int_in_slice(slice_of_ints *s, int x) {

}

#endif // KU_SLICE_H
