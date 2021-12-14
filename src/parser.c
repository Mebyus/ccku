#include <stdio.h>

#include "parser.h"

#define ENABLE_DEBUG 1
#include "debug.h"

const u8 parser_buffer_size = 2;

Token get_next_token(Parser *p) {
    Token token;
    if (p->prefetched) {
        p->prefetched = false;
        token         = p->prefetched_token;
    } else {
        token = scan_token(p->scanner);
        DEBUG(print_token(token);)
    }
    return token;
}

void advance_parser(Parser *p) {
    p->prev_token = p->token;
    p->token      = p->next_token;
    p->next_token = get_next_token(p);
}

void backup_advance_parser(Parser *p) {
    p->backup_token = p->prev_token;
    advance_parser(p);
}

void step_back_parser(Parser *p) {
    p->prefetched       = true;
    p->prefetched_token = p->next_token;
    p->next_token       = p->token;
    p->token            = p->prev_token;
    p->prev_token       = p->backup_token;
}

void skip_comments(Parser *p) {
    do {
        advance_parser(p);
    } while (p->token.type == tt_Comment);
}

Expression parse_expression(Parser *p) {
    Expression expr;
    if (p->token.type == tt_Identifier) {
        expr = init_identifier_expression(p->token);
        advance_parser(p);
        DEBUG(printf("identifier expression\n");)
        return expr;
    }
    if (p->token.type == tt_DecimalInteger) {
        expr = init_integer_expression(p->token);
        advance_parser(p);
        DEBUG(printf("integer expression\n");)
        return expr;
    }
    if (p->token.type == tt_String) {
        expr = init_string_expression(p->token);
        advance_parser(p);
        DEBUG(printf("string expression\n");)
        return expr;
    }
    return parse_expression(p);
}

Statement parse_define_statement(Parser *p) {
    slice_of_Expressions left = init_empty_slice_of_Expressions();
    append_Expression_to_slice(&left, parse_expression(p));

    advance_parser(p); // consume ":=" token

    slice_of_Expressions right = init_empty_slice_of_Expressions();
    append_Expression_to_slice(&right, parse_expression(p));

    advance_parser(p); // consume ";" token

    DEBUG(printf("define statement\n");)
    return init_define_statement(left, right);
}

Statement parse_call_statement(Parser *p) {
    Token name_token = p->token;

    advance_parser(p); // consume identifier token
    advance_parser(p); // consume "(" token

    slice_of_Expressions args = init_empty_slice_of_Expressions();

    append_Expression_to_slice(&args, parse_expression(p));

    advance_parser(p); // consume ")" token
    advance_parser(p); // consume ";" token

    DEBUG(printf("call statement\n");)
    return init_expression_statement(init_call_expression(name_token, args));
}

Statement parse_statement(Parser *p) {
    switch (p->token.type) {
    case tt_Identifier:
        if (p->next_token.type == tt_Define) {
            return parse_define_statement(p);
        }
        if (p->next_token.type == tt_LeftRoundBracket) {
            return parse_call_statement(p);
        }
        break;
    default:
        break;
    }
    advance_parser(p);

    DEBUG(printf("empty statement\n");)
    return init_empty_statement();
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
    slice_of_Statements s = init_empty_slice_of_Statements();
    advance_parser(p);
    advance_parser(p);
    while (p->token.type != tt_EOF) {
        Statement stmt = parse_statement(p);
        if (stmt.type != st_Empty) {
            append_Statement_to_slice(&s, stmt);
        }
    }
    return s;
}

bool is_top_level_start(TokenType type) {
    return type == tt_Function;
}

void parse_top_level(Parser *p) {}

void init_parser_buffer(Parser *p) {
    for (u8 i = 0; i < parser_buffer_size; i++) {
        advance_parser(p);
    }
}

StandaloneParseResult parse_standalone(Parser *p) {
    StandaloneParseResult result = {
        .ok = false,
    };
    result.tree = empty_standalone_source_text;

    while (p->token.type != tt_EOF) {
        if (p->token.type == tt_Comment) {
            skip_comments(p);
        } else if (is_toplevel_start(p->token.type)) {
            parse_top_level(p);
        } else {
            Statement stmt = parse_statement(p);
            if (stmt.type != st_Empty) {
                append_Statement_to_slice(&result.tree.statements, stmt);
            }
        }
    }

    return result;
}

StandaloneParseResult parse_standalone_source_from_str(str s) {
    Scanner scanner = init_scanner_from_str(s);
    Parser parser   = {
        .prefetched = false,
        .scanner    = &scanner,
    };
    init_parser_buffer(&parser);
    return parse_standalone(&parser);
}
