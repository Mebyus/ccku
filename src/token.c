#include <stdio.h>

#include "map.h"
#include "token.h"

const u8 min_keyword_length = 2;
const u8 max_keyword_length = 9;

const u32 lookup_token_map_cap = 1 << 8;

map_str_u64 lookup_token_map = empty_map;

str token_type_strings[] = {
    // Non static or empty literals
    [tt_Illegal]            = STR("ILLEGAL"),
    [tt_Comment]            = STR("COMMENT"),
    [tt_EOF]                = STR("EOF"),
    [tt_Terminator]         = STR("TERM"),
    [tt_Identifier]         = STR("IDENT"),
    [tt_String]             = STR("STR"),
    [tt_Character]          = STR("CHAR"),
    [tt_BinaryInteger]      = STR("BININT"),
    [tt_OctalInteger]       = STR("OCTINT"),
    [tt_DecimalInteger]     = STR("DECINT"),
    [tt_HexadecimalInteger] = STR("HEXINT"),
    [tt_DecimalFloat]       = STR("DECFLT"),

    // Operators/punctuators
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
    [tt_SubtractAssign]     = STR("-="),
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
        return token_type_strings[token.type];
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

TokenLookupResult lookup_keyword(str s) {
    TokenLookupResult result;
    if (s.len < min_keyword_length || s.len > max_keyword_length) {
        result.ok = false;
        return result;
    }

    for (int token_type = tt_begin_keyword + 1; token_type < tt_end_keyword; token_type++) {
        str keyword_str = token_type_strings[token_type];
        bool found      = are_strs_equal(s, keyword_str);
        if (found) {
            result.ok   = true;
            result.type = token_type;
            return result;
        }
    }

    result.ok = false;
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

map_str_u64 create_token_lookup_map() {
    map_str_u64 m = new_map_str_u64(lookup_token_map_cap);
    for (u64 type = tt_begin_no_static_literal + 1; type < tt_end_no_static_literal; type++) {
        put_map_str_u64(&m, token_type_strings[type], type);
    }
    for (u64 type = tt_begin_operator + 1; type < tt_end_operator; type++) {
        put_map_str_u64(&m, token_type_strings[type], type);
    }
    for (u64 type = tt_begin_keyword + 1; type < tt_end_keyword; type++) {
        put_map_str_u64(&m, token_type_strings[type], type);
    }
    put_map_str_u64(&m, token_type_strings[tt_Terminator], tt_Terminator);
    put_map_str_u64(&m, token_type_strings[tt_EOF], tt_EOF);
    return m;
}

TokenLookupResult lookup_token(str s) {
    if (lookup_token_map.buck == nil) {
        lookup_token_map = create_token_lookup_map();
    }
    TokenLookupResult lookup_res;
    result_u64 res  = get_map_str_u64(lookup_token_map, s);
    lookup_res.ok   = res.ok;
    lookup_res.type = res.val;
    return lookup_res;
}

TokenParseResult parse_token_from_str(str s) {
    TokenParseResult res;

    u64 start = 0;
    u64 end   = index_byte_in_str_from(s, ':', start);
    if (end >= s.len - 2) {
        res.ok = false;
        return res;
    }
    str line_str                            = borrow_str_slice(s, start, end);
    U32ParseResult line_number_parse_result = parse_u32_from_decimal(line_str);
    if (!line_number_parse_result.ok) {
        res.ok = false;
        return res;
    }
    u32 line = line_number_parse_result.num;

    start = end + 1;
    end   = index_byte_in_str_from(s, ' ', start);
    if (end >= s.len - 2) {
        res.ok = false;
        return res;
    }
    str column_str                            = borrow_str_slice(s, start, end);
    U32ParseResult column_number_parse_result = parse_u32_from_decimal(column_str);
    if (!column_number_parse_result.ok) {
        res.ok = false;
        return res;
    }
    u32 column = column_number_parse_result.num;

    start                                 = index_other_byte_in_str_from(s, ' ', end);
    end                                   = index_byte_in_str_from(s, ' ', start);
    str token_type_str                    = borrow_str_slice(s, start, end);
    TokenLookupResult token_lookup_result = lookup_token(token_type_str);
    if (!token_lookup_result.ok) {
        res.ok = false;
        return res;
    }
    TokenType token_type = token_lookup_result.type;

    if (has_static_literal(token_type)) {
        if (end >= s.len) {
            res.ok    = true;
            res.token = (Token){.type = token_type, .pos = {.line = line, .column = column}};
        } else {
            res.ok = false;
        }
        return res;
    }

    start = index_other_byte_in_str_from(s, ' ', end);
    if (start >= s.len) {
        res.ok = false;
        return res;
    }
    str token_literal = borrow_str_slice_to_end(s, start);

    res.ok    = true;
    res.token = (Token){.type = token_type, .pos = {.line = line, .column = column}, .literal = token_literal};
    return res;
}


void print_token(Token token) {
    str type_str   = token_type_strings[token.type];
    str line_str   = format_u32_as_decimal(token.pos.line);
    str column_str = format_u32_as_decimal(token.pos.column);

    print_str(line_str);
    fwrite(":", 1, 1, stdout);
    print_str(column_str);
    fwrite("  ", 1, 2, stdout);
    print_str(type_str);
    if (!has_static_literal(token.type)) {
        fwrite("  ", 1, 2, stdout);
        print_str(token.literal);
    }
    println();

    free_str(type_str);
    free_str(line_str);
    free_str(column_str);
}

void free_token(Token token) {
    free_str(token.literal);
}
