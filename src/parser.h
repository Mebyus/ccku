#ifndef KU_PARSER_H
#define KU_PARSER_H

#include "scanner.h"
#include "source.h"
#include "statement.h"

typedef struct Parser Parser;

struct Parser {
    // True means next token was prefetched and
    // we don't need to perform scan in order to
    // obtain it
    bool prefetched;

    Scanner *scanner;

    Token prev_token;
    Token next_token;
};

slice_of_Statements parse_str(str s);
slice_of_Statements parse_source(SourceText source);
slice_of_Statements parse_file(char *path);

slice_of_Statements parse(Parser *p);

#endif // KU_PARSER_H
