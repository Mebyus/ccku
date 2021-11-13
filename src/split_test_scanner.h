#ifndef KU_SPLIT_TEST_SCANNER_H
#define KU_SPLIT_TEST_SCANNER_H

#include "str.h"
#include "types.h"

typedef struct SplitTestScanner SplitTestScanner;

struct SplitTestScanner {
    bool control;
    u64 pos;
    str s;
    str next;
};

SplitTestScanner init_split_test_scanner(str s);
bool scan_next_split_test(SplitTestScanner *s);

#endif // KU_SPLIT_TEST_SCANNER_H
