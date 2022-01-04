#include <stdio.h>

#include "parser.h"

#define ENABLE_DEBUG 0
#include "debug.h"

const u8 parser_buffer_size = 2;

TypeSpecifier parse_type_specifier(Parser *p);

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

void parse_comments(Parser *p) {
    do { // just skip them for now
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

void terminate_parser(Parser *p, char *error_text) {
    fwrite(error_text, 1, strlen(error_text), stdout);
    println();
    print_token(p->token);
    exit(1);
}

TypeSpecifier parse_indexed_type_specifier(Parser *p) {
    advance_parser(p); // skip "["
    if (p->token.type != tt_RightSquareBracket) {
        terminate_parser(p, "\"]\" expected");
    }
    advance_parser(p); // skip "]"
    return parse_type_specifier(p);
}

TypeSpecifier parse_name_type_specifier(Parser *p) {
    TypeSpecifier type_specifier = new_name_type_specifier(p->token);
    advance_parser(p); // consume type name
    return type_specifier;
}

TypeSpecifier parse_type_specifier(Parser *p) {
    switch (p->token.type) {
    case tt_Identifier:
        return parse_name_type_specifier(p);
    case tt_Asterisk:
        terminate_parser(p, "pointer type specifiers are not implemented");
        break;
    case tt_LeftSquareBracket:
        return parse_indexed_type_specifier(p);
    case tt_Struct:
        terminate_parser(p, "struct type specifiers are not implemented");
        break;
    case tt_Channel:
        terminate_parser(p, "channel type specifiers are not implemented");
        break;
    default:
        terminate_parser(p, "unexpected token inside type specifier");
    }
    return empty_type_specifier; // non-reachable statement, avoids compiler warning
}

ParameterDeclaration parse_parameter_declaration(Parser *p) {
    ParameterDeclaration declaration = {
        .names = empty_slice_of_Identifiers,
    };
    while (true) {
        Identifier identifier = init_identifier(p->token);
        advance_parser(p); // consume parameter name
        append_Identifier_to_slice(&declaration.names, identifier);
        if (p->token.type == tt_Comma) {
            advance_parser(p); // skip ","
            if (p->token.type != tt_Identifier) {
                terminate_parser(p, "identifier expected");
            }
        } else if (p->token.type == tt_Colon) {
            advance_parser(p); // skip ":"
            declaration.type_specifier = parse_type_specifier(p);
            break;
        } else {
            terminate_parser(p, "unexpected token inside parameter declaration");
        }
    }
    return declaration;
}

FunctionParameters parse_function_parameters(Parser *p) {
    advance_parser(p); // skip "("
    FunctionParameters params = {
        .parameter_declarations = empty_slice_of_ParameterDeclarations,
    };
    while (p->token.type == tt_Identifier) {
        append_ParameterDeclaration_to_slice(&params.parameter_declarations, parse_parameter_declaration(p));
        if (p->token.type != tt_Comma) {
            break;
        }
        advance_parser(p); // skip ","
    }
    if (p->token.type != tt_RightRoundBracket) {
        terminate_parser(p, "\")\" expected");
    }
    advance_parser(p); // skip ")"
    return params;
}

FunctionResult parse_typed_tuple_function_result_from_colon(Parser *p, slice_of_Identifiers names) {
    advance_parser(p); // skip ":"
    slice_of_ParameterDeclarations parameter_declarations = empty_slice_of_ParameterDeclarations;
    ParameterDeclaration first_parameter_declaration      = {
        .names = names,
    };
    first_parameter_declaration.type_specifier = parse_type_specifier(p);
    append_ParameterDeclaration_to_slice(&parameter_declarations, first_parameter_declaration);

    if (p->token.type == tt_RightRoundBracket) {
        advance_parser(p); // skip ")"
        return new_typed_tuple_result(parameter_declarations);
    } else if (p->token.type != tt_Comma) {
        terminate_parser(p, "\")\" or \",\" expected");
    }
    advance_parser(p); // skip ","

    while (p->token.type == tt_Identifier) {
        append_ParameterDeclaration_to_slice(&parameter_declarations, parse_parameter_declaration(p));
        if (p->token.type != tt_Comma) {
            break;
        }
        advance_parser(p); // skip ","
    }
    if (p->token.type != tt_RightRoundBracket) {
        terminate_parser(p, "\")\" expected");
    }
    advance_parser(p); // skip ")"
    return new_typed_tuple_result(parameter_declarations);
}

FunctionResult parse_tuple_signature_function_result_from_type_specifier(Parser *p, slice_of_Identifiers names) {
    slice_of_TypeSpecifiers type_specifiers = new_type_specifiers_from_identifiers(names);
    append_TypeSpecifier_to_slice(&type_specifiers, parse_type_specifier(p));
    while (p->token.type == tt_Comma) {
        advance_parser(p); // skip ","
        if (p->token.type == tt_RightRoundBracket) {
            advance_parser(p); // skip ")"
            return new_tuple_signature_result(type_specifiers);
        }
        append_TypeSpecifier_to_slice(&type_specifiers, parse_type_specifier(p));
    }
    if (p->token.type != tt_RightRoundBracket) {
        terminate_parser(p, "\")\" expected");
    }
    advance_parser(p); // skip ")"
    return new_tuple_signature_result(type_specifiers);
}

FunctionResult parse_tuple_function_result(Parser *p) {
    advance_parser(p); // skip "("
    slice_of_Identifiers names = empty_slice_of_Identifiers;

    while (p->token.type == tt_Identifier) {
        append_Identifier_to_slice(&names, init_identifier(p->token));
        advance_parser(p); // consume name
        if (p->token.type != tt_Comma) {
            break;
        }
        advance_parser(p); // skip ","
    }

    if (p->token.type == tt_RightRoundBracket) {
        // result is a tuple signature

        FunctionResult function_result = new_tuple_signature_result_from_identifiers(names);
        free_slice_of_Identifiers(names);
        advance_parser(p); // skip ")"
        return function_result;
    }

    if (p->token.type == tt_Colon) {
        // result is a typed tuple

        return parse_typed_tuple_function_result_from_colon(p, names);
    }
    FunctionResult function_result = parse_tuple_signature_function_result_from_type_specifier(p, names);
    free_slice_of_Identifiers(names);
    return function_result;
}

FunctionResult parse_function_result(Parser *p) {
    if (p->token.type != tt_RightArrow) {
        return void_result;
    }
    advance_parser(p); // skip "=>"
    if (p->token.type != tt_LeftRoundBracket) {
        return new_simple_result(parse_type_specifier(p));
    }
    return parse_tuple_function_result(p);
}

FunctionDeclaration parse_function_declaration(Parser *p) {
    advance_parser(p); // skip "fn"
    if (p->token.type != tt_Identifier) {
        terminate_parser(p, "identifier expected");
    }
    FunctionDeclaration declaration = {
        .name = init_identifier(p->token),
    };
    advance_parser(p); // consume function name
    if (p->token.type != tt_LeftRoundBracket) {
        terminate_parser(p, "\"(\" expected");
    }
    declaration.parameters = parse_function_parameters(p);
    declaration.result     = parse_function_result(p);
    return declaration;
}

BlockStatement parse_block_statement(Parser *p) {
    BlockStatement block = {
        .statements = empty_slice_of_Statements,
    };
    advance_parser(p); // skip "{"
    while (p->token.type != tt_RightCurlyBracket && p->token.type != tt_EOF && p->token.type != tt_Illegal) {
        append_Statement_to_slice(&block.statements, parse_statement(p));
    }
    if (p->token.type != tt_RightCurlyBracket) {
        terminate_parser(p, "\"}\" expected");
    }
    advance_parser(p); // skip "}"
    return block;
}

void parse_function_definition(Parser *p) {
    FunctionDeclaration declaration = parse_function_declaration(p);
    if (p->token.type != tt_LeftCurlyBracket) {
        terminate_parser(p, "\"{\" expected");
    }
    BlockStatement body           = parse_block_statement(p);
    FunctionDefinition definition = {
        .declaration = declaration,
        .body        = body,
    };
    append_FunctionDefinition_to_slice(&p->source_tree.functions, definition);
}

void parse_top_level(Parser *p) {
    switch (p->token.type) {
    case tt_Comment:
        parse_comments(p);
        break;
    case tt_Function:
        parse_function_definition(p);
        break;
    default:
        parse_statement(p);
    }
}

void init_parser_buffer(Parser *p) {
    for (u8 i = 0; i < parser_buffer_size; i++) {
        advance_parser(p);
    }
}

StandaloneParseResult parse_standalone(Parser *p) {
    StandaloneParseResult result = {
        .ok = false,
    };
    result.tree = empty_standalone_source_tree;

    while (p->token.type != tt_EOF) {
        parse_top_level(p);
    }
    print_standalone_source_tree(p->source_tree);
    return result;
}

StandaloneParseResult parse_standalone_source_from_str(str s) {
    Scanner scanner = init_scanner_from_str(s);
    Parser parser   = {
        .prefetched  = false,
        .source_tree = empty_standalone_source_tree,
        .scanner     = &scanner,
    };
    init_parser_buffer(&parser);
    return parse_standalone(&parser);
}
