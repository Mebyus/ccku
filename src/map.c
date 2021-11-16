#include <stdlib.h>

#include "fatal.h"
#include "map.h"

map_str_u64 new_map_str_u64(u32 cap) {
    bucket_u64 **buck = (bucket_u64 **)calloc(cap, sizeof(bucket_u64 *));
    if (buck == nil) {
        fatal(1, "not enough memory for new map");
    }

    map_str_u64 m = {
        .cap  = cap,
        .len  = 0,
        .buck = buck,
    };
    return m;
}

bucket_u64 *new_bucket_u64(str key, u64 val) {
    bucket_u64 *buck = (bucket_u64 *)malloc(sizeof(bucket_u64));
    if (buck == nil) {
        fatal(1, "not enough memory for new bucket");
    }

    buck->key  = key;
    buck->val  = val;
    buck->next = nil;
    return buck;
}

u32 fnv_hash_1a_u32(byte *bytes, u64 len) {
    u32 h = 0x811c9dc5;
    for (u64 i = 0; i < len; i++) {
        h = (h ^ bytes[i]) * 0x01000193;
    }
    return h;
}

// unsigned long long fnv_hash_1a_64(void *key, int len) {
//     unsigned char *p     = key;
//     unsigned long long h = 0xcbf29ce484222325ULL;
//     int i;

//     for (i = 0; i < len; i++)
//         h = (h ^ p[i]) * 0x100000001b3ULL;

//     return h;
// }

u32 hash_str(str s, u32 cap) {
    return fnv_hash_1a_u32(s.bytes, s.len) % cap;
}

result_u64 get_map_str_u64(map_str_u64 m, str key) {
    result_u64 res;
    u32 h            = hash_str(key, m.cap);
    bucket_u64 *buck = m.buck[h];
    while (buck != nil) {
        if (are_strs_equal(key, buck->key)) {
            res.ok  = true;
            res.val = buck->val;
            return res;
        }
        buck = buck->next;
    }
    res.ok = false;
    return res;
}

void put_map_str_u64(map_str_u64 *m, str key, u64 val) {
    u32 h                 = hash_str(key, m->cap);
    bucket_u64 *buck      = m->buck[h];
    bucket_u64 *prev_buck = nil;
    while (buck != nil) {
        if (are_strs_equal(key, buck->key)) {
            buck->val = val;
            return;
        }
        prev_buck = buck;
        buck      = buck->next;
    }
    m->len++;
    bucket_u64 *new_buck = new_bucket_u64(key, val);
    if (prev_buck != nil) {
        m->col++;
        prev_buck->next = new_buck;
        return;
    }
    m->buck[h] = new_buck;
}