#include "split_test_scanner.h"

const str control_prefix = STR("##");

SplitTestScanner init_split_test_scanner(str s) {
    SplitTestScanner scanner = {
        .s   = s,
        .pos = 0,
    };
    return scanner;
}

bool scan_next_split_test(SplitTestScanner *s) {
    if (s->pos >= s->s.len) {
        return false;
    }
    if (has_substr_at(s->s, control_prefix, s->pos)) {
        s->control  = true;
        u64 new_pos = index_byte_in_str_from(s->s, '\n', s->pos);
        s->next     = borrow_str_slice(s->s, s->pos, new_pos);
        new_pos++;
        s->pos = new_pos;
        return true;
    }
    s->control  = false;
    u64 new_pos = s->pos;
    do {
        new_pos = index_byte_in_str_from(s->s, '\n', new_pos);
        new_pos++;
    } while (new_pos < s->s.len && s->s.bytes[new_pos] != '#');
    s->next = borrow_str_slice(s->s, s->pos, new_pos - 1);
    s->pos  = new_pos;
    return true;
}
