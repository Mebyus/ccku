#ifndef KU_BYTE_READER_H
#define KU_BYTE_READER_H

#include "str.h"

extern const int ReaderBOF;
extern const int ReaderEOF;

typedef struct StrByteReader StrByteReader;

struct StrByteReader {
    uint64_t offset;
    uint64_t mark;
    uint64_t pos;
    str s;
};

StrByteReader init_str_byte_reader(str s, uint64_t buffer_offset);
int read_next_code(StrByteReader *r);
void mark_str_byte_reader_position(StrByteReader *r);
str slice_from_str_byte_reader_mark(const StrByteReader *r);
void free_str_byte_reader(StrByteReader r);

#endif // KU_BYTE_READER_H
