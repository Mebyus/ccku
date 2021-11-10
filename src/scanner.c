#include <stdbool.h>
#include <stdlib.h>

#include "fatal.h"
#include "scanner.h"

const uint64_t scanner_buffer_size = 2;

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
    for (uint64_t i = 0; i < scanner_buffer_size; i++) {
        advance_scanner(s);
    }
}

Scanner *new_scanner_from_source(SourceText source) {
    Scanner *s = (Scanner *)malloc(sizeof(Scanner));
    if (s == NULL) {
        fatal(1, "not enough memory for new scanner");
    }

    s->prefetched = false;
    s->pos        = init_null_position();
    s->prev_code  = ReaderBOF;
    s->code       = ReaderBOF;
    s->next_code  = ReaderBOF;
    s->reader     = init_str_byte_reader(source.text);
    init_scanner_buffer(s);
    return s;
}

Scanner *new_scanner_from_str(str s) {
    return new_scanner_from_source(new_source_from_str(s));
}

Scanner init_scanner_from_source(SourceText source) {
    Scanner s = {
        .prefetched = false,
        .pos        = init_null_position(),
        .prev_code  = ReaderBOF,
        .code       = ReaderBOF,
        .next_code  = ReaderBOF,
        .reader     = init_str_byte_reader(source.text),
    };
    init_scanner_buffer(&s);
    return s;
}

Scanner init_scanner_from_str(str string) {
    Scanner s = {
        .prefetched = false,
        .pos        = init_null_position(),
        .prev_code  = ReaderBOF,
        .code       = ReaderBOF,
        .next_code  = ReaderBOF,
        .reader     = init_str_byte_reader(string),
    };
    init_scanner_buffer(&s);
    return s;
}


bool is_letter_or_underscore(uint8_t b) {
    return ('a' <= b && b <= 'z') || b == '_' || ('A' <= b && b <= 'Z');
}

bool is_alphanum(uint8_t b) {
    return ('a' <= b && b <= 'z') || b == '_' || ('A' <= b && b <= 'Z') || ('0' <= b && b <= '9');
}

bool is_letter(uint8_t b) {
    return ('a' <= b && b <= 'z') || ('A' <= b && b <= 'Z');
}

bool is_decimal_digit(uint8_t b) {
    return '0' <= b && b <= '9';
}

bool is_hexadecimal_digit(uint8_t b) {
    return ('0' <= b && b <= '9') || ('a' <= b && b <= 'f') || ('A' <= b && b <= 'F');
}

bool is_octal_digit(uint8_t b) {
    return '0' <= b && b <= '7';
}

bool is_binary_digit(uint8_t b) {
    return b == '0' || b == '1';
}

bool is_whitespace(uint8_t b) {
    return b == ' ' || b == '\n' || b == '\t' || b == '\r';
}

void skip_whitespace(Scanner *s) {
    while (s->code >= 0 && is_whitespace((uint8_t)s->code)) {
        advance_scanner(s);
    }
}

Token create_token_at_scanner_position(Scanner *s, TokenType type) {
    return create_token(type, s->pos);
}

void consume_word(Scanner *s) {
    do {
        advance_scanner(s);
    } while (s->code != ReaderEOF && is_alphanum((uint8_t)s->code));
}

Token scan_illegal_word(Scanner *s) {
    Token token = {
        .type = tt_Illegal,
        .pos  = s->pos,
    };
    uint64_t reader_start_pos = s->reader.pos - scanner_buffer_size;

    consume_word(s);

    if (s->code == ReaderEOF) {
        token.literal = new_str_slice_to_end(s->reader.s, reader_start_pos);
        return token;
    }

    uint64_t reader_end_pos = s->reader.pos - scanner_buffer_size;
    token.literal           = new_str_slice(s->reader.s, reader_start_pos, reader_end_pos);

    return token;
}

Token scan_name(Scanner *s) {
    Token token = {
        .pos = s->pos,
    };
    uint64_t reader_start_pos = s->reader.pos - scanner_buffer_size;

    consume_word(s);

    if (s->code == ReaderEOF) {
        token.literal = new_str_slice_to_end(s->reader.s, reader_start_pos);
    } else {
        uint64_t reader_end_pos = s->reader.pos - scanner_buffer_size;
        token.literal           = new_str_slice(s->reader.s, reader_start_pos, reader_end_pos);
    }

    KeywordLookupResult keyword_lookup_result = lookup_keyword(token.literal);
    if (keyword_lookup_result.found) {
        token.type = keyword_lookup_result.type;
    } else {
        token.type = tt_Identifier;
    }

    return token;
}

Token scan_binary_number(Scanner *s) {
    uint64_t start_pos = s->pos - 1;

    Token token = {
        .line   = s->line,
        .column = s->column - 1,
    };

    advance_scanner(s); // skip 'b' before the digits

    int code;
    do {
        code = advance_scanner(s);
    } while (code != text_eof && is_binary_digit((uint8_t)code));

    if (code != text_eof) {
        step_back_scanner(s);
    }

    uint64_t end_pos = s->pos;
    str literal      = new_str_slice(s->text, start_pos, end_pos);
    token.literal    = literal;
    token.type       = tt_Integer;

    return token;
}

Token scan_octal_number(Scanner *s) {
    uint64_t start_pos = s->pos - 1;

    Token token = {
        .line   = s->line,
        .column = s->column - 1,
    };

    advance_scanner(s); // skip 'o' before the digits

    int code;
    do {
        code = advance_scanner(s);
    } while (code != text_eof && is_octal_digit((uint8_t)code));

    if (code != text_eof) {
        step_back_scanner(s);
    }

    uint64_t end_pos = s->pos;
    str literal      = new_str_slice(s->text, start_pos, end_pos);
    token.literal    = literal;
    token.type       = tt_Integer;

    return token;
}

Token scan_decimal_number(Scanner *s) {
    Token token = {
        .pos = s->pos,
    };
    uint64_t reader_start_pos = s->reader.pos - scanner_buffer_size;

    do {
        advance_scanner(s);
    } while (s->code != ReaderEOF && is_decimal_digit((uint8_t)s->code));

    if (s->code == ReaderEOF) {
        token.literal = new_str_slice_to_end(s->reader.s, reader_start_pos);
        token.type    = tt_Integer;
        return token;
    } else if (is_alphanum((uint8_t)s->code)) {
        token.type = tt_Illegal;
        consume_word(s);

        if (s->code == ReaderEOF) {
            token.literal = new_str_slice_to_end(s->reader.s, reader_start_pos);
            return token;
        }

        uint64_t reader_end_pos = s->reader.pos - scanner_buffer_size;
        token.literal           = new_str_slice(s->reader.s, reader_start_pos, reader_end_pos);
        return token;
    }

    uint64_t reader_end_pos = s->reader.pos - scanner_buffer_size;
    token.literal           = new_str_slice(s->reader.s, reader_start_pos, reader_end_pos);
    token.type              = tt_Integer;

    return token;
}

Token scan_hexadecimal_number(Scanner *s) {
    uint64_t start_pos = s->pos - 1;

    Token token = {
        .line   = s->line,
        .column = s->column - 1,
    };

    advance_scanner(s); // skip 'x' before the digits

    int code;
    do {
        code = advance_scanner(s);
    } while (code != text_eof && is_hexadecimal_digit((uint8_t)code));

    if (code != text_eof) {
        step_back_scanner(s);
    }

    uint64_t end_pos = s->pos;
    str literal      = new_str_slice(s->text, start_pos, end_pos);
    token.literal    = literal;
    token.type       = tt_Integer;

    return token;
}

Token scan_number(Scanner *s) {
    Token token;

    if (s->code != '0') {
        return scan_decimal_number(s);
    }

    if (s->next_code == ReaderEOF) {
        token = create_token_with_literal(tt_Integer, s->pos, new_str_from_byte('0'));
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

    if (is_alphanum((uint8_t)s->next_code)) {
        token = scan_illegal_word(s);
        return token;
    }

    token = create_token_with_literal(tt_Integer, s->pos, new_str_from_byte('0'));
    return token;
}

Token scan_string_literal(Scanner *s) {
    Token token = {
        .pos = s->pos,
    };
    uint64_t reader_start_pos = s->reader.pos - scanner_buffer_size;

    do {
        advance_scanner(s);
    } while (s->code != ReaderEOF && (s->code != '"' || s->prev_code == '\\'));

    if (s->code != '"') {
        token.literal = new_str_slice_to_end(s->reader.s, reader_start_pos);
        token.type    = tt_Illegal;
        return token;
    }

    uint64_t reader_end_pos = s->reader.pos - scanner_buffer_size;
    token.literal           = new_str_slice(s->reader.s, reader_start_pos, reader_end_pos);
    token.type              = tt_String;
    advance_scanner(s);

    return token;
}

Token scan_line_comment(Scanner *s) {
    Token token = {
        .type = tt_Comment,
        .pos  = s->pos,
    };
    uint64_t reader_start_pos = s->reader.pos - scanner_buffer_size;

    do {
        advance_scanner(s);
    } while (s->code != ReaderEOF && s->code != '\n');

    if (s->code != '\n') {
        token.literal = new_str_slice_to_end(s->reader.s, reader_start_pos);
        return token;
    }

    uint64_t reader_end_pos = s->reader.pos - scanner_buffer_size;
    token.literal           = new_str_slice(s->reader.s, reader_start_pos, reader_end_pos);
    advance_scanner(s);

    return token;
}

Token scan_character_literal(Scanner *s) {
    Token token = {
        .pos = s->pos,
    };
    uint64_t reader_start_pos = s->reader.pos - scanner_buffer_size;

    do {
        advance_scanner(s);
    } while (s->code != ReaderEOF && s->code != '\'');

    if (s->code != '\'') {
        token.literal = new_str_slice_to_end(s->reader.s, reader_start_pos);
        token.type    = tt_Illegal;
        return token;
    }

    uint64_t reader_end_pos = s->reader.pos - scanner_buffer_size;
    token.literal           = new_str_slice(s->reader.s, reader_start_pos, reader_end_pos);
    token.type              = tt_Character;
    advance_scanner(s);

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

Token scan_other(Scanner *s) {
    switch (s->code) {
    case '(':
        return create_token_at_scanner_position(s, tt_LeftRoundBracket);
    case ')':
        return create_token_at_scanner_position(s, tt_RightRoundBracket);
    case '{':
        return create_token_at_scanner_position(s, tt_LeftCurlyBracket);
    case '}':
        return create_token_at_scanner_position(s, tt_RightCurlyBracket);
    case '[':
        return create_token_at_scanner_position(s, tt_LeftSquareBracket);
    case ']':
        return create_token_at_scanner_position(s, tt_RightSquareBracket);
    case '<':
        return create_token_at_scanner_position(s, tt_Less);
    case '>':
        return create_token_at_scanner_position(s, tt_Greater);
    case '+':
        return scan_plus_start(s);
    case ',':
        return create_token_at_scanner_position(s, tt_Comma);
    case '=':
        return create_token_at_scanner_position(s, tt_Assign);
    case ':':
        return scan_colon_start(s);
    case ';':
        return create_token_at_scanner_position(s, tt_Semicolon);
    case '.':
        return create_token_at_scanner_position(s, tt_Period);
    case '%':
        return create_token_at_scanner_position(s, tt_Percent);
    case '*':
        return create_token_at_scanner_position(s, tt_Asterisk);
    case '&':
        return create_token_at_scanner_position(s, tt_Ampersand);
    case '/':
        return create_token_at_scanner_position(s, tt_Slash);
    case '!':
        return create_token_at_scanner_position(s, tt_Not);
    default:
        return create_token_with_literal(tt_Illegal, s->pos, new_str_from_byte(s->code));
    }
}

Token scan_token(Scanner *s) {
    Token token;
    if (s->code == ReaderEOF) {
        token = create_token_at_scanner_position(s, tt_EOF);
        return token;
    }

    skip_whitespace(s);
    if (s->code == ReaderEOF) {
        token = create_token_at_scanner_position(s, tt_EOF);
        return token;
    }

    if (is_letter_or_underscore((uint8_t)s->code)) {
        token = scan_name(s);
        return token;
    }

    if (is_decimal_digit((uint8_t)s->code)) {
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
