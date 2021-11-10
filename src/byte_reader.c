#include <stdio.h>

#include "byte_reader.h"

#define ENABLE_DEBUG 1
#include "debug.h"

const int ReaderBOF = -1;
const int ReaderEOF = -2;

StrByteReader init_str_byte_reader(str s, uint64_t buffer_offset) {
    StrByteReader r = {
        .offset = buffer_offset,
        .pos    = 0,
        .mark   = 0,
        .s      = s,
    };
    DEBUG(printf("source len: %ld\n", s.len);)
    return r;
}

int read_next_code(StrByteReader *r) {
    if (r->pos >= r->s.len) {
        r->pos++;
        return ReaderEOF;
    }

    uint8_t b = r->s.bytes[r->pos];
    DEBUG(printf("reader pos: %ld, byte: %d\n", r->pos, b);)
    r->pos++;
    return (int)b;
}

void mark_str_byte_reader_position(StrByteReader *r) {
    if (r->pos < r->offset) {
        r->mark = 0;
        return;
    }
    r->mark = r->pos - r->offset;
}

str slice_from_str_byte_reader_mark(const StrByteReader *r) {
    if (r->pos > r->s.len + r->offset) {
        return new_str_slice_to_end(r->s, r->mark);
    }
    return new_str_slice(r->s, r->mark, r->pos - r->offset);
}

void free_str_byte_reader(StrByteReader r) {
    free_str(r.s);
}
