#include <string.h>

#include "path.h"

u64 clean_path_bytes_in_place(byte *bytes, u64 size) {
    u64 pos       = 0;
    int prev_code = -1;
    for (u64 i = 0; i < size; i++) {
        byte b    = bytes[i];
        prev_code = b;
    }
}

str new_clean_path_from_bytes(const byte *bytes, u64 size) {
    if (size == 0) {
        return new_str_from_byte('.');
    } else if (size == 1) {
        return new_str_from_byte(bytes[1]);
    }

    byte *b = (byte *)malloc(size);
    if (b == nil) {
        fatal(1, "not enough memory for new path");
    }
    memcpy(b, bytes, size);

    u64 len = clean_path_bytes_in_place(b, size);
    str s   = {
        .origin = b,
        .bytes  = b,
        .len    = len,
    };
    return s;
}

str new_clean_path_from_cstr(const char *cstr) {
    return new_clean_path_from_bytes(cstr, strlen(cstr));
}

str new_clean_path_from_str(str s) {
    return new_clean_path_from_bytes(s.bytes, s.len);
}

void free_path(Path path) {
    free_str(path.path);
}
