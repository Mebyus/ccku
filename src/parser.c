#include "parser.h"

Token advance(Parser *p) {
    Token token;
    if (p->prefetched) {
        p->prefetched = false;
        token         = p->next_token;
    } else {
        token = scan_next_token(p->scanner);
    }
    p->prev_token = token;
    print_token(token);
    return token;
}

Token peek(Parser *p) {
    Token token;
    if (p->prefetched) {
        token = p->next_token;
    } else {
        token         = scan_next_token(p->scanner);
        p->next_token = token;
        p->prefetched = true;
    }
    return token;
}

Expression parse_expression(Parser *p) {}

Statement parse_define_statement(Parser *p) {
    slice_of_Expressions left = new_null_slice_of_Expressions();
    append_Expression_to_slice(&left, get_identifier_expression(p->prev_token));

    advance(p); // swallow ":=" token

    slice_of_Expressions right = new_null_slice_of_Expressions();
    append_Expression_to_slice(&right, get_identifier_expression(advance(p)));

    advance(p); // swallow ";" token

    return get_define_statement(left, right);
}

Statement parse_call_statement(Parser *p) {
    Token name_token = p->prev_token;

    advance(p); // swallow "(" token

    slice_of_Expressions args = new_null_slice_of_Expressions();

    append_Expression_to_slice(&args, parse_expression(p));

    advance(p); // swallow ")" token
    advance(p); // swallow ";" token

    return get_expression_statement(get_call_expression(name_token, args));
}

Statement parse_statement(Parser *p) {
    Token token = advance(p);
    switch (token.type) {
    case tt_Identifier:
        Token next_token = peek(p);
        if (next_token.type == tt_Define) {
            return parse_define_statement(p);
        }
        if (next_token.type == tt_LeftRoundBracket) {
            return parse_call_statement(p);
        }
        break;
    default:
        break;
    }
    return get_empty_statement();
}

slice_of_Statements parse_source(SourceText source) {
    Parser parser = {
        .prefetched = false,
        .scanner    = new_scanner_from_source(source),
    };
    Parser *p = &parser;
    return parse(p);
}

slice_of_Statements parse(Parser *p) {
    slice_of_Statements s = new_null_slice_of_Statements();
    do {
        Statement stmt = parse_statement(p);
        if (stmt.type != st_Empty) {
            append_Statement_to_slice(&s, stmt);
        }
    } while (p->prev_token.type != tt_EOF);
    return s;
}
