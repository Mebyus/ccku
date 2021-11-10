#ifndef KU_BYTE_READER_H
#define KU_BYTE_READER_H

#include "str.h"

const int ReaderBOF = -1;
const int ReaderEOF = -2;

typedef struct StrByteReader StrByteReader;

struct StrByteReader {
    uint64_t pos;
    str s;
};

StrByteReader init_str_byte_reader(str s);
int read_next_code(StrByteReader *r);
void free_str_byte_reader(StrByteReader r);

#endif // KU_BYTE_READER_H
