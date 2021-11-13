#ifndef KU_MAP_H
#define KU_MAP_H

#include "str.h"
#include "types.h"

typedef struct map_str_u64 map_str_u64;
typedef struct bucket_u64 bucket_u64;
typedef struct result_u64 result_u64;

struct result_u64 {
    bool ok;
    u64 val;
};

struct bucket_u64 {
    u64 val;
    bucket_u64 *next;
    str key;
};

struct map_str_u64 {
    u32 cap;
    u32 len;
    bucket_u64 **buck;
};

map_str_u64 new_map_str_u64(u32 cap);
u32 hash_str(str s, u32 cap);
result_u64 map_str(map_str_u64 m, str key);
void put_map_str_u64(map_str_u64 *m, str key, u64 val);

#endif // KU_MAP_H
