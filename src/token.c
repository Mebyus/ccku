#include <stdio.h>

#include "map.h"
#include "token.h"

const u8 position_format_width   = 12;
const u8 token_type_format_width = 12;

const u32 lookup_token_map_cap = 1 << 8;
const byte *format_space_bytes = (byte *)"                    ";


// These variables should be initialized through init_token_module() call
map_str_u64 lookup_token_map = empty_map;
u64 min_keyword_length       = UINT64_MAX;
u64 max_keyword_length       = 0;

str token_type_strings[] = {
    // Non static or empty literals
    [tt_Empty]              = STR("EMPTY"),
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
    [tt_LeftArrow]          = STR("<-"),
    [tt_RightArrow]         = STR("=>"),
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

bool is_keyword(TokenType type) {
    return tt_begin_keyword < type && type < tt_end_keyword;
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

    result_u64 res = get_map_str_u64(lookup_token_map, s);
    if (!res.ok) {
        result.ok = false;
        return result;
    }

    if (!is_keyword(res.val)) {
        result.ok = false;
        return result;
    }

    result.ok   = true;
    result.type = res.val;
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

void init_token_lookup_map() {
    lookup_token_map = new_map_str_u64(lookup_token_map_cap);
    for (u64 type = tt_begin_no_static_literal + 1; type < tt_end_no_static_literal; type++) {
        put_map_str_u64(&lookup_token_map, token_type_strings[type], type);
    }
    for (u64 type = tt_begin_operator + 1; type < tt_end_operator; type++) {
        put_map_str_u64(&lookup_token_map, token_type_strings[type], type);
    }
    for (u64 type = tt_begin_keyword + 1; type < tt_end_keyword; type++) {
        put_map_str_u64(&lookup_token_map, token_type_strings[type], type);
    }
    put_map_str_u64(&lookup_token_map, token_type_strings[tt_Empty], tt_Empty);
    put_map_str_u64(&lookup_token_map, token_type_strings[tt_Terminator], tt_Terminator);
    put_map_str_u64(&lookup_token_map, token_type_strings[tt_EOF], tt_EOF);
}

void init_keyword_length_vars() {
    for (u64 type = tt_begin_keyword + 1; type < tt_end_keyword; type++) {
        u64 len = token_type_strings[type].len;
        if (min_keyword_length > len) {
            min_keyword_length = len;
        }
        if (max_keyword_length < len) {
            max_keyword_length = len;
        }
    }
}

TokenLookupResult lookup_token(str s) {
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

void init_token_module() {
    init_keyword_length_vars();
    init_token_lookup_map();
}

void print_token(Token token) {
    str type_str   = token_type_strings[token.type];
    str line_str   = format_u32_as_decimal(token.pos.line);
    str column_str = format_u32_as_decimal(token.pos.column);

    print_str(line_str);
    fwrite(":", 1, 1, stdout);
    print_str(column_str);
    u64 spaces = (u64)position_format_width - 1 - line_str.len - column_str.len;
    fwrite(format_space_bytes, 1, spaces, stdout);
    print_str(type_str);
    if (!has_static_literal(token.type)) {
        spaces = (u64)token_type_format_width - type_str.len;
        fwrite(format_space_bytes, 1, spaces, stdout);
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
