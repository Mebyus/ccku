#ifndef KU_SCANNER_H
#define KU_SCANNER_H

#include <stdbool.h>

#include "byte_reader.h"
#include "position.h"
#include "source.h"
#include "token.h"

typedef struct Scanner Scanner;

struct Scanner {
    bool prefetched;

    int backup_code;
    int prev_code;
    int code;
    int next_code;
    int prefetched_code;

    Position prev_pos;
    Position pos;

    StrByteReader reader;
};

Scanner *new_scanner_from_str(str s);
Scanner *new_scanner_from_source(SourceText source);
Scanner *new_scanner_from_file(char *path);

Scanner init_scanner_from_str(str s);
Scanner init_scanner_from_source(SourceText source);
Scanner init_scanner_from_file(char *path);

Token scan_token(Scanner *s);

void free_scanner(Scanner s);

#endif // KU_SCANNER_H
