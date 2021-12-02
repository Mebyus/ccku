#include <stdlib.h>
#include <string.h>

#include "fatal.h"
#include "path.h"

u64 clean_path_bytes_in_place(byte *bytes, u64 size) {
    u64 pos       = 0;
    int prev_code = -1;
    for (u64 i = 0; i < size; i++) {
        byte b = bytes[i];
        if (prev_code == b) {
        } else {
            pos++; // test
        }
        prev_code = b;
    }
    return pos;
}

str new_clean_path_from_bytes(const byte *bytes, u64 size) {
    if (size == 0) {
        return new_str_from_byte('.');
    } else if (size == 1) {
        return new_str_from_byte(bytes[1]);
    }

    byte *new_bytes = (byte *)malloc(size);
    if (new_bytes == nil) {
        fatal(1, "not enough memory for new path");
    }
    memcpy(new_bytes, bytes, size);

    u64 len = clean_path_bytes_in_place(new_bytes, size);
    str s   = {
        .origin = new_bytes,
        .bytes  = new_bytes,
        .len    = len,
    };
    return s;
}

str new_clean_path_from_cstr(const char *cstr) {
    return new_clean_path_from_bytes((byte *)cstr, strlen(cstr));
}

str new_clean_path_from_str(str s) {
    return new_clean_path_from_bytes(s.bytes, s.len);
}

void free_path(Path path) {
    free_str(path.path);
}
