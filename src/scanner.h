#ifndef KU_SCANNER_H
#define KU_SCANNER_H

#include <stdbool.h>

#include "source.h"
#include "token.h"

typedef struct Scanner Scanner;

struct Scanner {
    SourceText source;

    // Shortcut for source.text
    str text;

    // Scanner positioning
    bool started;
    uint64_t pos;
    uint32_t line;
    uint32_t column;

    // Last read byte
    uint8_t prev_byte;
    bool insert_semi;
};

Scanner *new_scanner_from_source(SourceText source);
Scanner *new_scanner_from_str(str s);
Token scan_next_token(Scanner *s);

void free_scanner(Scanner *s);

#endif // KU_SCANNER_H
