#include "byte_reader.h"

StrByteReader init_str_byte_reader(str s) {
    StrByteReader r = {
        .pos = 0,
        .s   = s,
    };
    return r;
}

int read_next_code(StrByteReader *r) {
    if (r->pos >= r->s.len) {
        return ReaderEOF;
    }

    uint8_t b = r->s.bytes[r->pos];
    r->pos++;
    return (int)b;
}

void free_str_byte_reader(StrByteReader r) {
    free_str(r.s);
}
