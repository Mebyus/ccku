#include <stdio.h>

#include "token.h"

const u8 min_keyword_length = 2;
const u8 max_keyword_length = 9;

str token_type_str[] = {
    [tt_Illegal]            = STR("ILLEGAL"),
    [tt_EOF]                = STR("EOF"),
    [tt_Terminator]         = STR("TERM"),
    [tt_BlankIdentifier]    = STR("_"),
    [tt_Plus]               = STR("+"),
    [tt_Minus]              = STR("-"),
    [tt_Equal]              = STR("=="),
    [tt_NotEqual]           = STR("!="),
    [tt_LessOrEqual]        = STR("<="),
    [tt_GreaterOrEqual]     = STR(">="),
    [tt_Increment]          = STR("++"),
    [tt_Decrement]          = STR("--"),
    [tt_Define]             = STR(":="),
    [tt_AddAssign]          = STR("+="),
    [tt_SubtractAssign]    = STR("-="),
    [tt_Assign]             = STR("="),
    [tt_Colon]              = STR(":"),
    [tt_Semicolon]          = STR(";"),
    [tt_Asterisk]           = STR("*"),
    [tt_Ampersand]          = STR("&"),
    [tt_Not]                = STR("!"),
    [tt_Slash]              = STR("/"),
    [tt_Percent]            = STR("%"),
    [tt_Period]             = STR("."),
    [tt_Comma]              = STR(","),
    [tt_Less]               = STR("<"),
    [tt_Greater]            = STR(">"),
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
    return type > tt_end_no_static_literal;
}

str get_token_literal(Token token) {
    if (has_static_literal(token.type)) {
        return token_type_str[token.type];
    }
    return token.literal;
}

Token create_token_with_literal(TokenType type, Position pos, str literal) {
    Token token = {
        .type    = type,
        .pos     = pos,
        .literal = literal,
    };
    return token;
}

Token create_token(TokenType type, Position pos) {
    Token token = {
        .type = type,
        .pos  = pos,
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

bool are_tokens_equal(Token t1, Token t2) {
    bool equal = (t1.type == t2.type) && are_positions_equal(t1.pos, t2.pos);
    if (!equal) {
        return false;
    }
    TokenType type = t1.type;
    if (has_static_literal(type)) {
        return true;
    }
    return are_strs_equal(t1.literal, t2.literal);
}

void print_token(Token token) {
    str literal    = get_token_literal(token);
    str type_str   = format_u64_as_decimal(token.type);
    str line_str   = format_u64_as_decimal(token.pos.line);
    str column_str = format_u64_as_decimal(token.pos.column);

    print_str(line_str);
    fwrite(":", 1, 1, stdout);
    print_str(column_str);
    fwrite("  ", 1, 2, stdout);
    print_str(type_str);
    fwrite("  ", 1, 2, stdout);
    println_str(literal);

    free_str(type_str);
    free_str(line_str);
    free_str(column_str);
}

void free_token(Token token) {
    free_str(token.literal);
}
