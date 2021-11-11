#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "charset.h"
#include "fatal.h"
#include "str.h"

const uint8_t max_uint32_decimal_length = 10;
const uint32_t max_small_decimal        = 99;

const uint8_t decimal_digits_string[10] = "0123456789";

const uint8_t small_decimals_string[200] = "00010203040506070809"
                                           "10111213141516171819"
                                           "20212223242526272829"
                                           "30313233343536373839"
                                           "40414243444546474849"
                                           "50515253545556575859"
                                           "60616263646566676869"
                                           "70717273747576777879"
                                           "80818283848586878889"
                                           "90919293949596979899";

str new_null_str() {
    str s = {
        .is_owner = false,
        .bytes    = NULL,
        .len      = 0,
    };
    return s;
}

bool is_null_str(str s) {
    return s.bytes == NULL;
}

str borrow_str_from_bytes_no_check(const uint8_t *bytes, uint64_t size) {
    str s = {
        .is_owner = false,
        .bytes    = (uint8_t *)(void *)bytes, // dirty trick to avoid compiler warning
        .len      = size,
    };
    return s;
}

str new_str_from_buf(char *buf, uint64_t size) {
    if (size == 0) {
        return new_null_str();
    } else if (size == 1) {
        return new_str_from_byte((uint8_t)buf[0]);
    }

    uint8_t *bytes = (uint8_t *)malloc(size);
    if (bytes == NULL) {
        fatal(1, "not enough memory for new string");
    }

    memcpy(bytes, buf, size);

    str s = {
        .is_owner = true,
        .bytes    = bytes,
        .len      = size,
    };
    return s;
}

str new_str_from_cstr(char *cstr) {
    uint64_t len = strlen(cstr);
    return new_str_from_buf(cstr, len);
}

str new_str_from_bytes(uint8_t *bytes, uint64_t size) {
    if (size == 0) {
        return new_null_str();
    } else if (size == 1) {
        return new_str_from_byte(bytes[0]);
    }

    uint8_t *b = (uint8_t *)malloc(size);
    if (b == NULL) {
        fatal(1, "not enough memory for new string");
    }

    memcpy(b, bytes, size);

    str s = {
        .is_owner = true,
        .bytes    = b,
        .len      = size,
    };
    return s;
}

str new_str_from_byte(uint8_t b) {
    return borrow_str_from_bytes_no_check(charset_bytes + b, 1);
}

str new_str_slice(str s, uint64_t start, uint64_t end) {
    return new_str_from_bytes(s.bytes + start, end - start);
}

str new_str_slice_to_end(str s, uint64_t start) {
    return new_str_slice(s, start, s.len);
}

str take_str_from_buf(char *buf, uint64_t size) {
    str s = {
        .is_owner = true,
        .bytes    = (uint8_t *)buf,
        .len      = size,
    };
    return s;
}

str take_str_from_bytes(uint8_t *bytes, uint64_t size) {
    str s = {
        .is_owner = true,
        .bytes    = bytes,
        .len      = size,
    };
    return s;
}

str take_str_from_cstr(char *cstr) {
    return take_str_from_buf(cstr, strlen(cstr));
}

str take_str_from_str(str *s) {
    str new_str = {
        .is_owner = s->is_owner,
        .bytes    = s->bytes,
        .len      = s->len,
    };
    s->is_owner = false;
    return new_str;
}

str borrow_str_from_bytes(const uint8_t *bytes, uint64_t size) {
    if (size == 0) {
        return new_null_str();
    }
    return borrow_str_from_bytes_no_check(bytes, size);
}

uint8_t decimal_digit_to_charcode(uint8_t digit) {
    return (uint8_t)(digit + '0');
}

uint8_t charcode_to_hexadecimal_digit(uint8_t code) {
    uint8_t d = (uint8_t)(code - '0');
    if (d < 10) {
        return d;
    }
    d = (uint8_t)(code - 'A' + 10);
    if (d < 16) {
        return d;
    }
    d = (uint8_t)(code - 'a' + 10);
    return d;
}

void reverse_bytes(uint8_t *bytes, uint64_t len) {
    uint64_t i, j;
    uint8_t b;

    for (i = 0, j = len - 1; i < j; i++, j--) {
        b        = bytes[i];
        bytes[i] = bytes[j];
        bytes[j] = b;
    }
}

uint8_t format_uint64_decimal_to_buf(uint64_t n, uint8_t *buf) {
    uint8_t i = 0;
    do { // generate digits in reverse order
        uint8_t digit = (uint8_t)(n % 10);
        n /= 10;
        buf[i] = decimal_digit_to_charcode(digit);
        i++;
    } while (n > 0);

    reverse_bytes(buf, i);
    return i;
}

str format_small_decimal(uint32_t n) {
    if (n < 10) {
        return borrow_str_from_bytes_no_check(decimal_digits_string + n, 1);
    }
    return borrow_str_from_bytes_no_check(small_decimals_string + n * 2, 2);
}

str format_uint64_as_decimal(uint32_t n) {
    if (n <= max_small_decimal) {
        return format_small_decimal(n);
    }

    uint8_t *bytes = (uint8_t *)malloc(max_uint32_decimal_length);
    if (bytes == NULL) {
        fatal(1, "not enough memory for new string");
    }

    uint64_t len = (uint64_t)format_uint64_decimal_to_buf(n, bytes);
    str s        = {
        .is_owner = true,
        .bytes    = bytes,
        .len      = len,
    };
    return s;
}

uint64_t parse_uint64_from_binary_no_checks(str s) {
    uint64_t num = 0;
    for (uint64_t i = 0; i < s.len; i++) {
        uint8_t bit = (uint8_t)(s.bytes[i] - '0');
        num <<= 1;
        num |= bit;
    }
    return num;
}

uint64_t parse_uint64_from_octal_no_checks(str s) {
    uint64_t num = 0;
    for (uint64_t i = 0; i < s.len; i++) {
        uint8_t bits = (uint8_t)(s.bytes[i] - '0');
        num <<= 3;
        num |= bits;
    }
    return num;
}

uint64_t parse_uint64_from_hexadecimal_no_checks(str s) {
    uint64_t num = 0;
    for (uint64_t i = 0; i < s.len; i++) {
        uint8_t bits = charcode_to_hexadecimal_digit(s.bytes[i]);
        num <<= 4;
        num |= bits;
    }
    return num;
}

str copy_str(str s) {
    if (s.len == 0) {
        return new_null_str();
    } else if (s.len == 1) {
        return new_str_from_byte(s.bytes[0]);
    }

    uint8_t *bytes = (uint8_t *)malloc(s.len);
    if (bytes == NULL) {
        fatal(1, "not enough memory for new string");
    }

    memcpy(bytes, s.bytes, s.len);

    str cp = {
        .is_owner = true,
        .bytes    = bytes,
        .len      = s.len,
    };
    return cp;
}

bool are_strs_equal(str s1, str s2) {
    if (s1.len != s2.len) {
        return false;
    }
    uint64_t len = s1.len;

    return memcmp(s1.bytes, s2.bytes, len) == 0;
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
    if (s.is_owner) {
        free(s.bytes);
    }
}
