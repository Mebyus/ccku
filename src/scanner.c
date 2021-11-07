#include <stdbool.h>
#include <stdlib.h>

#include "fatal.h"
#include "scanner.h"

const int text_eof = -1;

Scanner *new_scanner_from_source(SourceText source) {
    Scanner *scanner = (Scanner *)malloc(sizeof(Scanner));
    if (scanner == NULL) {
        fatal(1, "not enough memory for new scanner");
    }

    scanner->column      = 1;
    scanner->line        = 1;
    scanner->pos         = 0;
    scanner->source      = source;
    scanner->text        = source.text;
    scanner->started     = false;
    scanner->insert_semi = false;

    return scanner;
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

int advance(Scanner *s) {
    if (s->pos >= s->text.len) {
        return text_eof;
    }

    uint8_t b = s->text.bytes[s->pos];
    if (b == '\n') {
        s->column = 1;
        s->line++;
    } else {
        s->column++;
    }

    s->pos++;
    s->prev_byte = b;
    return (int)b;
}

int peek(Scanner *s) {
    if (s->pos < s->text.len) {
        uint8_t b = s->text.bytes[s->pos];
        return (int)b;
    }

    return text_eof;
}

uint8_t step_back(Scanner *s) {
    s->pos--;
    if (s->prev_byte == '\n') {
        s->line--;
    }
    s->column--;
    return s->prev_byte;
}

int skip_whitespace(Scanner *s) {
    int code;
    do {
        code = advance(s);
    } while (code != text_eof && is_whitespace((uint8_t)code));

    if (code == text_eof) {
        return text_eof;
    }

    return step_back(s);
}

Token scan_illegal_word(Scanner *s) {
    uint64_t start_pos = s->pos - 1;

    Token token = {
        .line   = s->line,
        .column = s->column - 1,
    };

    int code;
    do {
        code = advance(s);
    } while (code != text_eof && is_alphanum((uint8_t)code));

    if (code != text_eof) {
        step_back(s);
    }

    uint64_t end_pos = s->pos;
    str literal      = new_str_slice(s->text, start_pos, end_pos);
    token.literal    = literal;
    token.type       = tt_Illegal;

    return token;
}

Token scan_name(Scanner *s) {
    uint64_t start_pos = s->pos - 1;

    Token token = {
        .line   = s->line,
        .column = s->column - 1,
    };

    int code;
    do {
        code = advance(s);
    } while (code != text_eof && is_alphanum((uint8_t)code));

    if (code != text_eof) {
        step_back(s);
    }

    uint64_t end_pos = s->pos;
    str literal      = new_str_slice(s->text, start_pos, end_pos);
    token.literal    = literal;

    KeywordLookupResult keyword_lookup_result = lookup_keyword(literal);
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

    advance(s); // skip 'b' before the digits

    int code;
    do {
        code = advance(s);
    } while (code != text_eof && is_binary_digit((uint8_t)code));

    if (code != text_eof) {
        step_back(s);
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

    advance(s); // skip 'o' before the digits

    int code;
    do {
        code = advance(s);
    } while (code != text_eof && is_octal_digit((uint8_t)code));

    if (code != text_eof) {
        step_back(s);
    }

    uint64_t end_pos = s->pos;
    str literal      = new_str_slice(s->text, start_pos, end_pos);
    token.literal    = literal;
    token.type       = tt_Integer;

    return token;
}

Token scan_decimal_number(Scanner *s) {
    uint64_t start_pos = s->pos - 1;

    Token token = {
        .line   = s->line,
        .column = s->column - 1,
    };

    int code;
    do {
        code = advance(s);
    } while (code != text_eof && is_decimal_digit((uint8_t)code));

    if (code != text_eof) {
        step_back(s);
    }

    uint64_t end_pos = s->pos;
    str literal      = new_str_slice(s->text, start_pos, end_pos);
    token.literal    = literal;
    token.type       = tt_Integer;

    return token;
}

Token scan_hexadecimal_number(Scanner *s) {
    uint64_t start_pos = s->pos - 1;

    Token token = {
        .line   = s->line,
        .column = s->column - 1,
    };

    advance(s); // skip 'x' before the digits

    int code;
    do {
        code = advance(s);
    } while (code != text_eof && is_hexadecimal_digit((uint8_t)code));

    if (code != text_eof) {
        step_back(s);
    }

    uint64_t end_pos = s->pos;
    str literal      = new_str_slice(s->text, start_pos, end_pos);
    token.literal    = literal;
    token.type       = tt_Integer;

    return token;
}

Token scan_number(Scanner *s) {
    Token token;

    uint8_t b = s->prev_byte;
    if (b != '0') {
        return scan_decimal_number(s);
    }

    int code = peek(s);

    if (code == text_eof) {
        token = create_token_with_literal(s->line, s->column - 1, tt_Integer, new_str_from_byte('0'));
        return token;
    }

    if (code == 'b') {
        token = scan_binary_number(s);
        return token;
    }

    if (code == 'o') {
        token = scan_octal_number(s);
        return token;
    }

    if (code == 'x') {
        token = scan_hexadecimal_number(s);
        return token;
    }

    if (is_alphanum((uint8_t)code)) {
        token = scan_illegal_word(s);
        return token;
    }

    token = create_token_with_literal(s->line, s->column - 1, tt_Integer, new_str_from_byte('0'));
    return token;
}

Token scan_other(Scanner *s) {
    Token token;

    uint32_t column_start = s->column - 1;
    uint8_t b             = s->prev_byte;
    switch (b) {
    case '(':
        token = create_token(s->line, column_start, tt_LeftRoundBracket);
        break;
    case ')':
        token = create_token(s->line, column_start, tt_RightRoundBracket);
        break;
    case '{':
        token = create_token(s->line, column_start, tt_LeftCurlyBracket);
        break;
    case '}':
        token = create_token(s->line, column_start, tt_RightCurlyBracket);
        break;
    case '+':
        token = create_token(s->line, column_start, tt_Plus);
        break;
    case ',':
        token = create_token(s->line, column_start, tt_Comma);
        break;
    default:
        token = create_token_with_literal(s->line, column_start, tt_Illegal, new_str_from_byte(b));
        break;
    }

    return token;
}

Token scan_next_token(Scanner *s) {
    Token token;
    int code = peek(s);
    if (code == text_eof) {
        token = create_token(s->line, s->column, tt_EOF);
        return token;
    }

    uint8_t b;
    if (is_whitespace((uint8_t)code)) {
        code = skip_whitespace(s);
        if (code == text_eof) {
            // skip_whitespace() returns whitespace only if EOF was reached.
            // Advance scanner before we return EOF token, so that next peek()
            // returns EOF
            token = create_token(s->line, s->column, tt_EOF);
            return token;
        }
    }
    b = (uint8_t)advance(s);

    // bool insert_semi = false;

    if (is_letter_or_underscore(b)) {
        token = scan_name(s);
        return token;
    }

    if (is_decimal_digit(b)) {
        token = scan_number(s);
        return token;
    }

    token = scan_other(s);
    return token;
}

void free_scanner(Scanner *s) {
    free_source(s->source);
    free(s);
}