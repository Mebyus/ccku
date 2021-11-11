#ifndef KU_STR_H
#define KU_STR_H

#include <stdbool.h>
#include <stdint.h>

#define STR(s)                                                                                                         \
    { .bytes = (uint8_t *)s, .len = sizeof(s) - 1, .is_owner = false }

typedef struct str str;

struct str {
    bool is_owner;
    uint8_t *bytes;
    uint64_t len;
};

str new_null_str();
str new_str_from_cstr(char *s);
str new_str_from_buf(char *buf, uint64_t size);
str new_str_from_bytes(uint8_t *bytes, uint64_t size);
str new_str_from_byte(uint8_t b);
str new_str_slice(str s, uint64_t start, uint64_t end);
str new_str_slice_to_end(str s, uint64_t start);
str take_str_from_cstr(char *s);
str take_str_from_buf(char *buf, uint64_t size);
str take_str_from_bytes(uint8_t *bytes, uint64_t size);
str take_str_from_str(str *s);
str borrow_str_from_bytes(const uint8_t *bytes, uint64_t size);
str copy_str(str s);
str format_uint64_as_decimal(uint32_t n);
uint64_t parse_uint64_from_binary_no_checks(str s);
uint64_t parse_uint64_from_octal_no_checks(str s);
uint64_t parse_uint64_from_hexadecimal_no_checks(str s);
bool is_null_str(str s);
bool are_strs_equal(str s1, str s2);
char *str_to_cstr(str s);
void free_str(str s);

#endif // KU_STR_H
