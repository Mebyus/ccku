#include "strop.h"

IMPLEMENT_SLICE(str)

slice_of_strs borrow_split_str_by_byte(str s, byte b) {
    slice_of_strs slice = empty_slice_of_strs;
    u64 start           = 0;
    for (u64 i = 0; i < s.len; i++) {
        if (s.bytes[i] == b) {
            if (i > start) {
                str substr = borrow_str_slice(s, start, i);
                append_str_to_slice(&slice, substr);
            }
            start = i + 1;
        }
    }
    if (start < s.len) {
        str substr = borrow_str_slice_to_end(s, start);
        append_str_to_slice(&slice, substr);
    }
    return slice;
}
