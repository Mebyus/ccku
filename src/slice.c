#include "slice.h"

const uint32_t first_grow_threshold  = 1 << 16;
const uint32_t second_grow_threshold = 1 << 20;
const uint32_t third_grow_threshold  = 1 << 24;
const uint32_t forth_grow_threshold  = 1 << 28;

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
