#include <stdlib.h>

#include "fatal.h"
#include "scanner.h"

const u8 scanner_buffer_size = 2;

int get_next_code(Scanner *s) {
    int code;
    if (s->prefetched) {
        s->prefetched = false;
        code          = s->prefetched_code;
    } else {
        code = read_next_code(&s->reader);
    }
    return code;
}

void advance_scanner(Scanner *s) {
    if (s->code >= 0) {
        if (s->code == '\n') {
            s->pos = next_line(s->pos);
        } else {
            s->pos = next_column(s->pos);
        }
    }
    s->prev_code = s->code;
    s->code      = s->next_code;
    s->next_code = get_next_code(s);
}

void backup_advance_scanner(Scanner *s) {
    s->prev_pos    = s->pos;
    s->backup_code = s->prev_code;
    advance_scanner(s);
}

void step_back_scanner(Scanner *s) {
    s->prefetched      = true;
    s->prefetched_code = s->next_code;
    s->next_code       = s->code;
    s->code            = s->prev_code;
    s->prev_code       = s->backup_code;
    s->pos             = s->prev_pos;
}

void init_scanner_buffer(Scanner *s) {
    for (u8 i = 0; i < scanner_buffer_size; i++) {
        advance_scanner(s);
    }
}

Scanner *new_scanner_from_source(SourceText source) {
    Scanner *s = (Scanner *)malloc(sizeof(Scanner));
    if (s == nil) {
        fatal(1, "not enough memory for new scanner");
    }

    s->prefetched        = false;
    s->insert_terminator = false;
    s->insert_blocked    = false;
    s->pos               = init_null_position();
    s->prev_code         = ReaderBOF;
    s->code              = ReaderBOF;
    s->next_code         = ReaderBOF;
    s->reader            = init_str_byte_reader(source.text, scanner_buffer_size);
    init_scanner_buffer(s);
    return s;
}

Scanner *new_scanner_from_str(str s) {
    return new_scanner_from_source(new_source_from_str(s));
}

Scanner init_scanner_from_source(SourceText source) {
    Scanner s = {
        .prefetched        = false,
        .insert_terminator = false,
        .insert_blocked    = false,
        .pos               = init_null_position(),
        .prev_code         = ReaderBOF,
        .code              = ReaderBOF,
        .next_code         = ReaderBOF,
        .reader            = init_str_byte_reader(source.text, scanner_buffer_size),
    };
    init_scanner_buffer(&s);
    return s;
}

Scanner init_scanner_from_str(str string) {
    Scanner s = {
        .prefetched        = false,
        .insert_terminator = false,
        .insert_blocked    = false,
        .pos               = init_null_position(),
        .prev_code         = ReaderBOF,
        .code              = ReaderBOF,
        .next_code         = ReaderBOF,
        .reader            = init_str_byte_reader(string, scanner_buffer_size),
    };
    init_scanner_buffer(&s);
    return s;
}


bool is_letter_or_underscore(byte b) {
    return ('a' <= b && b <= 'z') || b == '_' || ('A' <= b && b <= 'Z');
}

bool is_alphanum(byte b) {
    return ('a' <= b && b <= 'z') || b == '_' || ('A' <= b && b <= 'Z') || ('0' <= b && b <= '9');
}

bool is_letter(byte b) {
    return ('a' <= b && b <= 'z') || ('A' <= b && b <= 'Z');
}

bool is_decimal_digit(byte b) {
    return '0' <= b && b <= '9';
}

bool is_decimal_digit_or_period(byte b) {
    return ('0' <= b && b <= '9') || b == '.';
}

bool is_hexadecimal_digit(byte b) {
    return ('0' <= b && b <= '9') || ('a' <= b && b <= 'f') || ('A' <= b && b <= 'F');
}

bool is_octal_digit(byte b) {
    return '0' <= b && b <= '7';
}

bool is_binary_digit(byte b) {
    return b == '0' || b == '1';
}

bool is_whitespace(byte b) {
    return b == ' ' || b == '\n';
}

bool is_terminator_token(TokenType type) {
    return type == tt_Identifier || type == tt_Return || type == tt_Break || type == tt_Continue;
}

bool does_token_block_terminator(TokenType type) {
    return type == tt_Function || type == tt_If || type == tt_Else || type == tt_ElseIf || type == tt_Loop ||
           type == tt_For || type == tt_While || type == tt_Switch;
}

void skip_whitespace(Scanner *s) {
    while (s->code >= 0 && is_whitespace((byte)s->code)) {
        advance_scanner(s);
    }
}

void skip_space(Scanner *s) {
    while (s->code >= 0 && s->code == ' ') {
        advance_scanner(s);
    }
}

Token create_token_at_scanner_position(Scanner *s, TokenType type) {
    return create_token(type, s->pos);
}

void consume_word(Scanner *s) {
    do {
        advance_scanner(s);
    } while (s->code != ReaderEOF && is_alphanum((byte)s->code));
}

Token scan_illegal_word(Scanner *s) {
    Token token = {
        .type = tt_Illegal,
        .pos  = s->pos,
    };
    mark_str_byte_reader_position(&s->reader);
    consume_word(s);
    token.literal = slice_from_str_byte_reader_mark(&s->reader);
    return token;
}

Token scan_name(Scanner *s) {
    Token token = {
        .pos = s->pos,
    };
    mark_str_byte_reader_position(&s->reader);
    consume_word(s);
    token.literal = slice_from_str_byte_reader_mark(&s->reader);

    TokenLookupResult keyword_lookup_result = lookup_keyword(token.literal);
    if (keyword_lookup_result.found) {
        token.type = keyword_lookup_result.type;
    } else {
        token.type = tt_Identifier;
    }

    if (!s->insert_blocked && is_terminator_token(token.type)) {
        s->insert_terminator = true;
    }
    if (does_token_block_terminator(token.type)) {
        s->insert_terminator = false;
        s->insert_blocked    = true;
    }
    return token;
}

Token scan_binary_number(Scanner *s) {
    Token token = {
        .pos = s->pos,
    };
    mark_str_byte_reader_position(&s->reader);

    advance_scanner(s); // skip '0' byte
    advance_scanner(s); // skip 'b' byte

    bool scanned_at_least_one_digit = false;
    while (s->code != ReaderEOF && is_binary_digit((byte)s->code)) {
        advance_scanner(s);
        scanned_at_least_one_digit = true;
    }

    if (is_alphanum((byte)s->code)) {
        consume_word(s);
        token.type    = tt_Illegal;
        token.literal = slice_from_str_byte_reader_mark(&s->reader);
        return token;
    }

    if (!scanned_at_least_one_digit) {
        token.type    = tt_Illegal;
        token.literal = slice_from_str_byte_reader_mark(&s->reader);
        return token;
    }

    token.type    = tt_BinaryInteger;
    token.literal = slice_from_str_byte_reader_mark(&s->reader);
    return token;
}

Token scan_octal_number(Scanner *s) {
    Token token = {
        .pos = s->pos,
    };
    mark_str_byte_reader_position(&s->reader);

    advance_scanner(s); // skip '0' byte
    advance_scanner(s); // skip 'o' byte

    bool scanned_at_least_one_digit = false;
    while (s->code != ReaderEOF && is_octal_digit((byte)s->code)) {
        advance_scanner(s);
        scanned_at_least_one_digit = true;
    }

    if (is_alphanum((byte)s->code)) {
        consume_word(s);
        token.type    = tt_Illegal;
        token.literal = slice_from_str_byte_reader_mark(&s->reader);
        return token;
    }

    if (!scanned_at_least_one_digit) {
        token.type    = tt_Illegal;
        token.literal = slice_from_str_byte_reader_mark(&s->reader);
        return token;
    }

    token.type    = tt_OctalInteger;
    token.literal = slice_from_str_byte_reader_mark(&s->reader);
    return token;
}

Token scan_decimal_number(Scanner *s) {
    Token token = {
        .pos = s->pos,
    };
    mark_str_byte_reader_position(&s->reader);

    bool scanned_one_period = false;
    do {
        advance_scanner(s);
        if (s->code == '.') {
            if (scanned_one_period) {
                break;
            } else {
                scanned_one_period = true;
            }
        }
    } while (s->code != ReaderEOF && is_decimal_digit_or_period((byte)s->code));

    if (is_alphanum((byte)s->code) || s->code == '.') {
        consume_word(s);
        token.type    = tt_Illegal;
        token.literal = slice_from_str_byte_reader_mark(&s->reader);
        return token;
    }

    if (s->prev_code == '.') {
        token.type    = tt_Illegal;
        token.literal = slice_from_str_byte_reader_mark(&s->reader);
        return token;
    }

    if (scanned_one_period) {
        token.type = tt_DecimalFloat;
    } else {
        token.type = tt_DecimalInteger;
    }
    token.literal = slice_from_str_byte_reader_mark(&s->reader);
    return token;
}

Token scan_hexadecimal_number(Scanner *s) {
    Token token = {
        .pos = s->pos,
    };
    mark_str_byte_reader_position(&s->reader);

    advance_scanner(s); // skip '0' byte
    advance_scanner(s); // skip 'x' byte

    bool scanned_at_least_one_digit = false;
    while (s->code != ReaderEOF && is_hexadecimal_digit((byte)s->code)) {
        advance_scanner(s);
        scanned_at_least_one_digit = true;
    }

    if (is_alphanum((byte)s->code)) {
        consume_word(s);
        token.type    = tt_Illegal;
        token.literal = slice_from_str_byte_reader_mark(&s->reader);
        return token;
    }

    if (!scanned_at_least_one_digit) {
        token.type    = tt_Illegal;
        token.literal = slice_from_str_byte_reader_mark(&s->reader);
        return token;
    }

    token.type    = tt_HexadecimalInteger;
    token.literal = slice_from_str_byte_reader_mark(&s->reader);
    return token;
}

Token scan_number(Scanner *s) {
    Token token;

    if (s->code != '0') {
        return scan_decimal_number(s);
    }

    if (s->next_code == ReaderEOF) {
        token = create_token_with_literal(tt_DecimalInteger, s->pos, new_str_from_byte('0'));
        advance_scanner(s);
        return token;
    }

    if (s->next_code == 'b') {
        token = scan_binary_number(s);
        return token;
    }

    if (s->next_code == 'o') {
        token = scan_octal_number(s);
        return token;
    }

    if (s->next_code == 'x') {
        token = scan_hexadecimal_number(s);
        return token;
    }

    if (s->next_code == '.') {
        token = scan_decimal_number(s);
        return token;
    }

    if (is_alphanum((byte)s->next_code)) {
        token = scan_illegal_word(s);
        return token;
    }

    token = create_token_with_literal(tt_DecimalInteger, s->pos, new_str_from_byte('0'));
    advance_scanner(s);
    return token;
}

Token scan_string_literal(Scanner *s) {
    Token token = {
        .pos = s->pos,
    };
    mark_str_byte_reader_position(&s->reader);

    do {
        advance_scanner(s);
    } while (s->code != ReaderEOF && (s->code != '"' || s->prev_code == '\\'));

    if (s->code != '"') {
        token.type    = tt_Illegal;
        token.literal = slice_from_str_byte_reader_mark(&s->reader);
        return token;
    }

    advance_scanner(s);
    token.type    = tt_String;
    token.literal = slice_from_str_byte_reader_mark(&s->reader);

    return token;
}

Token scan_line_comment(Scanner *s) {
    Token token = {
        .type = tt_Comment,
        .pos  = s->pos,
    };
    mark_str_byte_reader_position(&s->reader);

    do {
        advance_scanner(s);
    } while (s->code != ReaderEOF && s->code != '\n');

    token.literal = slice_from_str_byte_reader_mark(&s->reader);
    if (s->code != '\n') {
        return token;
    }
    advance_scanner(s);
    return token;
}

Token scan_character_literal(Scanner *s) {
    Token token = {
        .pos = s->pos,
    };
    mark_str_byte_reader_position(&s->reader);

    do {
        advance_scanner(s);
    } while (s->code != ReaderEOF && s->code != '\'');

    if (s->code != '\'') {
        token.type    = tt_Illegal;
        token.literal = slice_from_str_byte_reader_mark(&s->reader);
        return token;
    }

    advance_scanner(s);
    token.type    = tt_Character;
    token.literal = slice_from_str_byte_reader_mark(&s->reader);
    return token;
}

Token scan_colon_start(Scanner *s) {
    Token token;

    if (s->next_code == '=') {
        token = create_token_at_scanner_position(s, tt_Define);
        advance_scanner(s);
        advance_scanner(s);
    } else {
        token = create_token_at_scanner_position(s, tt_Colon);
        advance_scanner(s);
    }

    return token;
}

Token scan_equal_sign_start(Scanner *s) {
    Token token;

    if (s->next_code == '=') {
        token = create_token_at_scanner_position(s, tt_Equal);
        advance_scanner(s);
        advance_scanner(s);
    } else {
        token = create_token_at_scanner_position(s, tt_Assign);
        advance_scanner(s);
    }

    return token;
}

Token scan_less_start(Scanner *s) {
    Token token;

    if (s->next_code == '=') {
        token = create_token_at_scanner_position(s, tt_LessOrEqual);
        advance_scanner(s);
        advance_scanner(s);
    } else {
        token = create_token_at_scanner_position(s, tt_Less);
        advance_scanner(s);
    }

    return token;
}

Token scan_greater_start(Scanner *s) {
    Token token;

    if (s->next_code == '=') {
        token = create_token_at_scanner_position(s, tt_GreaterOrEqual);
        advance_scanner(s);
        advance_scanner(s);
    } else {
        token = create_token_at_scanner_position(s, tt_Greater);
        advance_scanner(s);
    }

    return token;
}

Token scan_ampersand_start(Scanner *s) {
    Token token;

    if (s->next_code == '&') {
        token = create_token_at_scanner_position(s, tt_LogicalAnd);
        advance_scanner(s);
        advance_scanner(s);
    } else {
        token = create_token_at_scanner_position(s, tt_Ampersand);
        advance_scanner(s);
    }

    return token;
}

Token scan_not_start(Scanner *s) {
    Token token;

    if (s->next_code == '=') {
        token = create_token_at_scanner_position(s, tt_NotEqual);
        advance_scanner(s);
        advance_scanner(s);
    } else {
        token = create_token_at_scanner_position(s, tt_Not);
        advance_scanner(s);
    }

    return token;
}

Token scan_pipe_start(Scanner *s) {
    Token token;

    if (s->next_code == '|') {
        token = create_token_at_scanner_position(s, tt_LogicalOr);
        advance_scanner(s);
        advance_scanner(s);
    } else {
        token = create_token_at_scanner_position(s, tt_Pipe);
        advance_scanner(s);
    }

    return token;
}

Token scan_left_curly_bracket(Scanner *s) {
    Token token;

    s->insert_blocked = false;
    token             = create_token_at_scanner_position(s, tt_LeftCurlyBracket);
    advance_scanner(s);

    return token;
}

Token scan_plus_start(Scanner *s) {
    Token token;

    if (s->next_code == '=') {
        token = create_token_at_scanner_position(s, tt_AddAssign);
        advance_scanner(s);
        advance_scanner(s);
    } else if (s->next_code == '+') {
        token = create_token_at_scanner_position(s, tt_Increment);
        advance_scanner(s);
        advance_scanner(s);
    } else {
        token = create_token_at_scanner_position(s, tt_Plus);
        advance_scanner(s);
    }

    return token;
}

Token scan_minus_start(Scanner *s) {
    Token token;

    if (s->next_code == '=') {
        token = create_token_at_scanner_position(s, tt_SubtractAssign);
        advance_scanner(s);
        advance_scanner(s);
    } else if (s->next_code == '-') {
        token = create_token_at_scanner_position(s, tt_Decrement);
        advance_scanner(s);
        advance_scanner(s);
    } else {
        token = create_token_at_scanner_position(s, tt_Minus);
        advance_scanner(s);
    }

    return token;
}

Token scan_single_byte_token(Scanner *s, TokenType type) {
    Token token = create_token_at_scanner_position(s, type);
    advance_scanner(s);
    return token;
}

Token scan_illegal_byte_token(Scanner *s) {
    Token token = create_token_with_literal(tt_Illegal, s->pos, new_str_from_byte((byte)s->code));
    advance_scanner(s);
    return token;
}

// scan_other scans next operator, punctucator or illegal byte Token
Token scan_other(Scanner *s) {
    switch (s->code) {
    case '(':
        return scan_single_byte_token(s, tt_LeftRoundBracket);
    case ')':
        return scan_single_byte_token(s, tt_RightRoundBracket);
    case '{':
        return scan_left_curly_bracket(s);
    case '}':
        return scan_single_byte_token(s, tt_RightCurlyBracket);
    case '[':
        return scan_single_byte_token(s, tt_LeftSquareBracket);
    case ']':
        return scan_single_byte_token(s, tt_RightSquareBracket);
    case '<':
        return scan_less_start(s);
    case '>':
        return scan_greater_start(s);
    case '+':
        return scan_plus_start(s);
    case '-':
        return scan_minus_start(s);
    case ',':
        return scan_single_byte_token(s, tt_Comma);
    case '=':
        return scan_equal_sign_start(s);
    case ':':
        return scan_colon_start(s);
    case ';':
        return scan_single_byte_token(s, tt_Semicolon);
    case '.':
        return scan_single_byte_token(s, tt_Period);
    case '%':
        return scan_single_byte_token(s, tt_Percent);
    case '*':
        return scan_single_byte_token(s, tt_Asterisk);
    case '&':
        return scan_ampersand_start(s);
    case '/':
        return scan_single_byte_token(s, tt_Slash);
    case '!':
        return scan_not_start(s);
    case '|':
        return scan_pipe_start(s);
    default:
        return scan_illegal_byte_token(s);
    }
}

// scan_token scans next Token
Token scan_token(Scanner *s) {
    Token token;
    if (s->code == ReaderEOF) {
        if (s->insert_terminator) {
            s->insert_terminator = false;
            token                = create_token_at_scanner_position(s, tt_Terminator);
        } else {
            token = create_token_at_scanner_position(s, tt_EOF);
        }
        return token;
    }

    if (s->insert_terminator) {
        skip_space(s);
        if (s->code == ReaderEOF || s->code == '}' || (s->code == '/' && s->next_code == '/')) {
            s->insert_terminator = false;
            token                = create_token_at_scanner_position(s, tt_Terminator);
            return token;
        }
        if (s->code == '\n') {
            s->insert_terminator = false;
            token                = create_token_at_scanner_position(s, tt_Terminator);
            advance_scanner(s);
            return token;
        }
    } else {
        skip_whitespace(s);
        if (s->code == ReaderEOF) {
            token = create_token_at_scanner_position(s, tt_EOF);
            return token;
        }
    }

    if (is_letter_or_underscore((byte)s->code)) {
        token = scan_name(s);
        return token;
    }

    if (is_decimal_digit((byte)s->code)) {
        token = scan_number(s);
        return token;
    }

    if (s->code == '"') {
        token = scan_string_literal(s);
        return token;
    }

    if (s->code == '/' && s->next_code == '/') {
        token = scan_line_comment(s);
        return token;
    }

    if (s->code == '\'') {
        token = scan_character_literal(s);
        return token;
    }

    token = scan_other(s);
    return token;
}

void free_scanner(Scanner s) {
    free_str_byte_reader(s.reader);
}
