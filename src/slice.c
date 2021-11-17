#include "slice.h"

const u32 first_grow_threshold  = 1 << 16;
const u32 second_grow_threshold = 1 << 20;
const u32 third_grow_threshold  = 1 << 24;
const u32 forth_grow_threshold  = 1 << 28;

const u32 max_growable_cap = 0b11110000111100001111000011110000;

u32 get_new_cap(u32 cap) {
    if (cap > max_growable_cap) {
        fatal(1, "slice capacity limit reached");
    }
    u32 new_cap;
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
