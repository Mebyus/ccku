#include <stdio.h>

#include "token.h"

const uint64_t min_keyword_length = 2;
const uint64_t max_keyword_length = 9;

str token_type_str[] = {
    [tt_Illegal]            = STR("ILLEGAL"),
    [tt_EOF]                = STR("EOF"),
    [tt_BlankIdentifier]    = STR("_"),
    [tt_Plus]               = STR("+"),
    [tt_Minus]              = STR("-"),
    [tt_Asterisk]           = STR("*"),
    [tt_Slash]              = STR("/"),
    [tt_Percent]            = STR("%"),
    [tt_Period]             = STR("."),
    [tt_Comma]              = STR(","),
    [tt_LeftCurlyBracket]   = STR("{"),
    [tt_RightCurlyBracket]  = STR("}"),
    [tt_LeftRoundBracket]   = STR("("),
    [tt_RightRoundBracket]  = STR(")"),
    [tt_LeftSquareBracket]  = STR("["),
    [tt_RightSquareBracket] = STR("]"),

    // Keywords
    [tt_Import]    = STR("import"),
    [tt_Function]  = STR("fn"),
    [tt_Continue]  = STR("continue"),
    [tt_Return]    = STR("return"),
    [tt_Break]     = STR("break"),
    [tt_Const]     = STR("const"),
    [tt_Case]      = STR("case"),
    [tt_For]       = STR("for"),
    [tt_Else]      = STR("else"),
    [tt_If]        = STR("if"),
    [tt_ElseIf]    = STR("elif"),
    [tt_Defer]     = STR("defer"),
    [tt_Default]   = STR("default"),
    [tt_Interface] = STR("interface"),
    [tt_Map]       = STR("map"),
    [tt_Module]    = STR("module"),
    [tt_Dirty]     = STR("dirty"),
    [tt_Immutable] = STR("imt"),
    [tt_In]        = STR("in"),
    [tt_Var]       = STR("var"),
    [tt_Type]      = STR("type"),
    [tt_Switch]    = STR("switch"),
    [tt_Struct]    = STR("struct"),
    [tt_Public]    = STR("pub"),
    [tt_Ku]        = STR("ku"),
    [tt_Select]    = STR("select"),
    [tt_Package]   = STR("package"),
    [tt_Goto]      = STR("goto"),
    [tt_Channel]   = STR("chan"),
    [tt_Loop]      = STR("loop"),
    [tt_While]     = STR("while"),
};

bool has_static_literal(TokenType type) {
    return type != tt_Comment && type != tt_Identifier && type != tt_Integer && type != tt_Float &&
           type != tt_Character && type != tt_String && type != tt_Illegal;
}

str get_token_literal(Token token) {
    if (has_static_literal(token.type)) {
        return token_type_str[token.type];
    }
    return token.literal;
}

Token create_token_with_literal(uint32_t line, uint32_t column, TokenType type, str literal) {
    Token token = {
        .line    = line,
        .column  = column,
        .type    = type,
        .literal = literal,
    };
    return token;
}

Token create_token(uint32_t line, uint32_t column, TokenType type) {
    Token token = {
        .line   = line,
        .column = column,
        .type   = type,
    };
    return token;
}

KeywordLookupResult lookup_keyword(str s) {
    KeywordLookupResult result;
    if (s.len < min_keyword_length || s.len > max_keyword_length) {
        result.found = false;
        return result;
    }

    for (int token_type = tt_begin_keyword + 1; token_type < tt_end_keyword; token_type++) {
        str keyword_str = token_type_str[token_type];
        bool found      = are_strs_equal(s, keyword_str);
        if (found) {
            result.found = true;
            result.type  = token_type;
            return result;
        }
    }

    result.found = false;
    return result;
}

void print_token(Token token) {
    str literal    = get_token_literal(token);
    str type_str   = format_uint32_decimal(token.type);
    str line_str   = format_uint32_decimal(token.line);
    str column_str = format_uint32_decimal(token.column);

    fwrite(line_str.bytes, 1, line_str.len, stdout);
    fwrite(":", 1, 1, stdout);
    fwrite(column_str.bytes, 1, column_str.len, stdout);
    fwrite("  ", 1, 2, stdout);
    fwrite(type_str.bytes, 1, type_str.len, stdout);
    fwrite("  ", 1, 2, stdout);
    fwrite(literal.bytes, 1, literal.len, stdout);
    fwrite("\n", 1, 1, stdout);

    free_str(type_str);
    free_str(line_str);
    free_str(column_str);
}

void free_token(Token token) {
    free_str(token.literal);
}
