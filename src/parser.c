#include "parser.h"

slice_of_Statements parse_source(SourceText source) {
    Parser parser = {
        .scanner = new_scanner_from_source(source),
    };
    Parser *p = &parser;
    return parse(p);
}

slice_of_Statements parse(Parser *p) {
    slice_of_Statements s = new_null_slice_of_Statements();
    Token token;
    do {
        token = scan_next_token(p->scanner);
        print_token(token);
    } while (token.type != tt_EOF);
    return s;
}
