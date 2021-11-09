#ifndef KU_PARSER_H
#define KU_PARSER_H

#include "scanner.h"
#include "source.h"
#include "statement.h"

typedef struct Parser Parser;

struct Parser {
    Scanner *scanner;
};

slice_of_Statements parse_str(str s);
slice_of_Statements parse_source(SourceText source);
slice_of_Statements parse_file(char *path);

slice_of_Statements parse(Parser *p);

#endif // KU_PARSER_H
