#ifndef KU_STR_H
#define KU_STR_H

#include "types.h"

#define STR(s)                                                                                                         \
    { .bytes = (byte *)s, .len = sizeof(s) - 1, .is_owner = false }

typedef struct str str;
typedef struct U64ParseResult U64ParseResult;

struct str {
    bool is_owner;
    byte *bytes;
    u64 len;
};

struct U64ParseResult {
    bool ok;
    u64 num;
};

extern const str empty_str;

str new_str_from_cstr(char *s);
str new_str_from_buf(char *buf, u64 size);
str new_str_from_bytes(byte *bytes, u64 size);
str new_str_from_byte(byte b);
str new_str_slice(str s, u64 start, u64 end);
str new_str_slice_to_end(str s, u64 start);
str take_str_from_cstr(char *s);
str take_str_from_buf(char *buf, u64 size);
str take_str_from_bytes(byte *bytes, u64 size);
str take_str_from_str(str *s);
str borrow_str_from_bytes(const byte *bytes, u64 size);
str borrow_str_slice(str s, u64 start, u64 end);
str borrow_str_slice_to_end(str s, u64 start);
str copy_str(str s);
str format_u32_as_decimal(u32 n);
str format_u64_as_decimal(u64 n);
str format_i32_as_decimal(i32 n);
str format_i64_as_decimal(i64 n);
u64 parse_uint64_from_binary_no_checks(str s);
u64 parse_uint64_from_octal_no_checks(str s);
u64 parse_uint64_from_hexadecimal_no_checks(str s);
U64ParseResult parse_uint64_from_decimal(str s);
bool is_empty_str(str s);
bool are_strs_equal(str s1, str s2);
bool has_prefix_str(str s, str prefix);
bool has_substr_at(str s, str substr, u64 pos);
u64 index_byte_in_str(str s, byte b);
u64 index_byte_in_str_from(str s, byte b, u64 pos);
u64 index_other_byte_in_str(str s, byte b);
u64 index_other_byte_in_str_from(str s, byte b, u64 pos);
char *str_to_cstr(str s);
void print_str(str s);
void println_str(str s);
void println();
void free_str(str s);

#endif // KU_STR_H
