#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fatal.h"
#include "str.h"

const uint8_t max_uint32_decimal_length = 10;
const uint8_t zero_charcode             = '0';

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

str new_str_from_buf(char *buf, uint64_t size) {
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
    uint8_t *bytes = (uint8_t *)malloc(1);
    if (bytes == NULL) {
        fatal(1, "not enough memory for new string");
    }

    bytes[0] = b;

    str s = {
        .is_owner = true,
        .bytes    = bytes,
        .len      = 1,
    };
    return s;
}

str new_str_slice(str s, uint64_t start, uint64_t end) {
    return new_str_from_bytes(s.bytes + start, end - start);
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

uint8_t decimal_digit_to_charcode(uint8_t digit) {
    return (uint8_t)(digit + zero_charcode);
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

str format_uint32_decimal(uint32_t n) {
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

str copy_str(str s) {
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

void free_str(str s) {
    if (s.is_owner) {
        free(s.bytes);
    }
}
