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

u32 hash_str(str s, u32 cap) {
    u32 h = 0;
    for (u64 i = 0; i < s.len; i++) {
        h += s.bytes[i];
    }
    return h % cap;
}

result_u64 map_str(map_str_u64 m, str key) {
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
    bucket_u64 *new_buck = new_bucket_u64(key, val);
    if (prev_buck != nil) {
        prev_buck->next = new_buck;
        return;
    }
    m->buck[h] = new_buck;
}