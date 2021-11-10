#include <stdio.h>

#include "byte_reader.h"

#define ENABLE_DEBUG 1
#include "debug.h"

const int ReaderBOF = -1;
const int ReaderEOF = -2;

StrByteReader init_str_byte_reader(str s) {
    StrByteReader r = {
        .pos = 0,
        .s   = s,
    };
    DEBUG(printf("source len: %ld\n", s.len);)
    return r;
}

int read_next_code(StrByteReader *r) {
    if (r->pos >= r->s.len) {
        return ReaderEOF;
    }

    uint8_t b = r->s.bytes[r->pos];
    DEBUG(printf("reader pos: %ld, byte: %d\n", r->pos, b);)
    r->pos++;
    return (int)b;
}

void free_str_byte_reader(StrByteReader r) {
    free_str(r.s);
}
