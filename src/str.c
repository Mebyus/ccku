#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "charset.h"
#include "fatal.h"
#include "str.h"

const u8 max_u32_decimal_length = 10;
const u8 max_u64_decimal_length = 20;

const u8 max_small_decimal = 99;

const byte decimal_digits_string[10] = "0123456789";

const byte small_decimals_string[200] = "00010203040506070809"
                                        "10111213141516171819"
                                        "20212223242526272829"
                                        "30313233343536373839"
                                        "40414243444546474849"
                                        "50515253545556575859"
                                        "60616263646566676869"
                                        "70717273747576777879"
                                        "80818283848586878889"
                                        "90919293949596979899";

const str empty_str = {.origin = nil, .bytes = nil, .len = 0};

bool is_empty_str(str s) {
    return s.len == 0;
}

str borrow_str_from_bytes_no_check(const byte *bytes, u64 size) {
    str s = {
        .origin = nil,
        .bytes  = (byte *)(void *)bytes, // dirty trick to avoid compiler warning
        .len    = size,
    };
    return s;
}

str new_str_from_chars(char *buf, u64 size) {
    if (size == 0) {
        return empty_str;
    } else if (size == 1) {
        return new_str_from_byte((byte)buf[0]);
    }

    byte *bytes = (byte *)malloc(size);
    if (bytes == nil) {
        fatal(1, "not enough memory for new string");
    }

    memcpy(bytes, buf, size);

    str s = {
        .origin = bytes,
        .bytes  = bytes,
        .len    = size,
    };
    return s;
}

str new_str_from_cstr(char *cstr) {
    u64 len = strlen(cstr);
    return new_str_from_chars(cstr, len);
}

str new_str_from_bytes(byte *bytes, u64 size) {
    if (size == 0) {
        return empty_str;
    } else if (size == 1) {
        return new_str_from_byte(bytes[0]);
    }

    byte *b = (byte *)malloc(size);
    if (b == nil) {
        fatal(1, "not enough memory for new string");
    }

    memcpy(b, bytes, size);

    str s = {
        .origin = b,
        .bytes  = b,
        .len    = size,
    };
    return s;
}

str new_str_from_byte(byte b) {
    return borrow_str_from_bytes_no_check(charset_bytes + b, 1);
}

str new_str_slice(str s, u64 start, u64 end) {
    return new_str_from_bytes(s.bytes + start, end - start);
}

str new_str_slice_to_end(str s, u64 start) {
    return new_str_slice(s, start, s.len);
}

str take_str_from_chars(char *buf, u64 size) {
    str s = {
        .origin = buf,
        .bytes  = (byte *)buf,
        .len    = size,
    };
    return s;
}

str take_str_from_bytes(byte *bytes, u64 size) {
    str s = {
        .origin = bytes,
        .bytes  = bytes,
        .len    = size,
    };
    return s;
}

str take_str_from_cstr(char *cstr) {
    return take_str_from_chars(cstr, strlen(cstr));
}

str take_str_from_str(str *s) {
    str new_str = {
        .origin = s->origin,
        .bytes  = s->bytes,
        .len    = s->len,
    };
    s->origin = nil;
    return new_str;
}

str borrow_str_from_bytes(const byte *bytes, u64 size) {
    if (size == 0) {
        return empty_str;
    }
    return borrow_str_from_bytes_no_check(bytes, size);
}

str borrow_str_slice(str s, u64 start, u64 end) {
    return borrow_str_from_bytes(s.bytes + start, end - start);
}

str borrow_str_slice_from_start(str s, u64 end) {
    return borrow_str_slice(s, 0, end);
}

str borrow_str_slice_to_end(str s, u64 start) {
    return borrow_str_slice(s, start, s.len);
}

byte decimal_digit_to_charcode(u8 digit) {
    return (byte)(digit + '0');
}

u8 charcode_to_hexadecimal_digit(byte code) {
    u8 d = (u8)(code - '0');
    if (d < 10) {
        return d;
    }
    d = (u8)(code - 'A' + 10);
    if (d < 16) {
        return d;
    }
    d = (u8)(code - 'a' + 10);
    return d;
}

void reverse_bytes(byte *bytes, u64 len) {
    u64 i, j;
    byte b;

    for (i = 0, j = len - 1; i < j; i++, j--) {
        b        = bytes[i];
        bytes[i] = bytes[j];
        bytes[j] = b;
    }
}

u8 format_u64_decimal_to_buf(u64 n, byte *buf) {
    u8 i = 0;
    do { // generate digits in reverse order
        u8 digit = (u8)(n % 10);
        n /= 10;
        buf[i] = decimal_digit_to_charcode(digit);
        i++;
    } while (n > 0);

    reverse_bytes(buf, i);
    return i;
}

str format_small_decimal(u8 n) {
    if (n < 10) {
        return borrow_str_from_bytes_no_check(decimal_digits_string + n, 1);
    }
    return borrow_str_from_bytes_no_check(small_decimals_string + n * 2, 2);
}

str format_u32_as_decimal(u32 n) {
    if (n <= max_small_decimal) {
        return format_small_decimal((u8)n);
    }

    byte *bytes = (byte *)malloc(max_u32_decimal_length);
    if (bytes == nil) {
        fatal(1, "not enough memory for new string");
    }

    u64 len = (u64)format_u64_decimal_to_buf(n, bytes);
    str s   = {
        .origin = bytes,
        .bytes  = bytes,
        .len    = len,
    };
    return s;
}

str format_u64_as_decimal(u64 n) {
    if (n <= max_small_decimal) {
        return format_small_decimal((u8)n);
    }

    byte *bytes = (byte *)malloc(max_u64_decimal_length);
    if (bytes == nil) {
        fatal(1, "not enough memory for new string");
    }

    u64 len = (u64)format_u64_decimal_to_buf(n, bytes);
    str s   = {
        .origin = bytes,
        .bytes  = bytes,
        .len    = len,
    };
    return s;
}

u64 parse_u64_from_binary_no_checks(str s) {
    u64 num = 0;
    for (u64 i = 0; i < s.len; i++) {
        u8 bit = (u8)(s.bytes[i] - '0');
        num <<= 1;
        num |= bit;
    }
    return num;
}

u64 parse_u64_from_octal_no_checks(str s) {
    u64 num = 0;
    for (u64 i = 0; i < s.len; i++) {
        u8 bits = (u8)(s.bytes[i] - '0');
        num <<= 3;
        num |= bits;
    }
    return num;
}

u64 parse_u64_from_hexadecimal_no_checks(str s) {
    u64 num = 0;
    for (u64 i = 0; i < s.len; i++) {
        u8 bits = charcode_to_hexadecimal_digit(s.bytes[i]);
        num <<= 4;
        num |= bits;
    }
    return num;
}

U32ParseResult parse_u32_from_decimal(str s) {
    U32ParseResult res;
    if (s.len == 0 || s.len > max_u32_decimal_length) {
        res.ok = false;
        return res;
    }
    u32 num = 0;
    for (u64 i = 0; i < s.len; i++) {
        num *= 10;
        byte b = s.bytes[i];
        if (b < '0' || b > '9') {
            res.ok = false;
            return res;
        }
        u8 digit = (u8)(b - '0');
        num += digit;
    }
    res.ok  = true;
    res.num = num;
    return res;
}

U64ParseResult parse_u64_from_decimal(str s) {
    U64ParseResult res;
    if (s.len == 0 || s.len > max_u64_decimal_length) {
        res.ok = false;
        return res;
    }
    u64 num = 0;
    for (u64 i = 0; i < s.len; i++) {
        num *= 10;
        byte b = s.bytes[i];
        if (b < '0' || b > '9') {
            res.ok = false;
            return res;
        }
        u8 digit = (u8)(b - '0');
        num += digit;
    }
    res.ok  = true;
    res.num = num;
    return res;
}

str new_str_from_str(str s) {
    if (s.len == 0) {
        return empty_str;
    } else if (s.len == 1) {
        return new_str_from_byte(s.bytes[0]);
    }

    byte *bytes = (byte *)malloc(s.len);
    if (bytes == nil) {
        fatal(1, "not enough memory for new string");
    }

    memcpy(bytes, s.bytes, s.len);

    str cp = {
        .origin = bytes,
        .bytes  = bytes,
        .len    = s.len,
    };
    return cp;
}

bool are_strs_equal(str s1, str s2) {
    if (s1.len != s2.len) {
        return false;
    }
    return memcmp(s1.bytes, s2.bytes, s1.len) == 0;
}

bool has_prefix_str(str s, str prefix) {
    if (prefix.len > s.len) {
        return false;
    }
    return memcmp(s.bytes, prefix.bytes, prefix.len) == 0;
}

bool has_substr_at(str s, str substr, u64 pos) {
    if (substr.len + pos > s.len) {
        return false;
    }
    return memcmp(s.bytes + pos, substr.bytes, substr.len) == 0;
}

u64 index_other_byte_in_str(str s, byte b) {
    return index_other_byte_in_str_from(s, b, 0);
}

u64 index_other_byte_in_str_from(str s, byte b, u64 pos) {
    u64 i;
    for (i = pos; i < s.len && s.bytes[i] == b; i++) {
    }
    return i;
}

u64 index_byte_in_str(str s, byte b) {
    return index_byte_in_str_from(s, b, 0);
}

u64 index_byte_in_str_from(str s, byte b, u64 pos) {
    u64 i;
    for (i = pos; i < s.len && s.bytes[i] != b; i++) {
    }
    return i;
}

void print_str(str s) {
    if (s.len == 0) {
        return;
    }
    fwrite(s.bytes, 1, s.len, stdout);
}

void println_str(str s) {
    print_str(s);
    fwrite("\n", 1, 1, stdout);
}

void println() {
    fwrite("\n", 1, 1, stdout);
};

void free_str(str s) {
    if (s.origin == nil) {
        return;
    }
    free(s.origin);
}
