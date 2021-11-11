#include "scanner.h"
#include "slice.h"

TYPEDEF_SLICE(Token);
IMPLEMENT_SLICE(Token);

typedef struct ScannerTestCase ScannerTestCase;

struct ScannerTestCase {
    str label;
    str source_str;
    slice_of_Tokens want_tokens;
};

TYPEDEF_SLICE(ScannerTestCase);
IMPLEMENT_SLICE(ScannerTestCase);

const str warn_str = STR("Tokens do not match in test case: ");
const str want_str = STR("Want: ");
const str got_str  = STR("Got:  ");

int main() {
    ScannerTestCase test_case1 = {
        .label       = STR("empty source"),
        .source_str  = new_null_str(),
        .want_tokens = SLICE(Token, {{.type = tt_EOF, .pos = {.line = 1, .column = 1}}}),
    };
    ScannerTestCase test_case2 = {
        .label       = STR("decimal integer"),
        .source_str  = STR("325290"),
        .want_tokens = SLICE(Token,
            {
                {.type = tt_DecimalInteger, .literal = STR("325290"), .pos = {.line = 1, .column = 1}},
                {.type = tt_EOF, .pos = {.line = 1, .column = 7}},
            }),
    };
    ScannerTestCase test_case3 = {
        .label       = STR("decimal float"),
        .source_str  = STR("325.290"),
        .want_tokens = SLICE(Token,
            {
                {.type = tt_DecimalFloat, .literal = STR("325.290"), .pos = {.line = 1, .column = 1}},
                {.type = tt_EOF, .pos = {.line = 1, .column = 8}},
            }),
    };
    slice_of_ScannerTestCases test_cases = SLICE(ScannerTestCase, {test_case1, test_case2, test_case3});
    println();
    for (uint32_t i = 0; i < test_cases.len; i++) {
        ScannerTestCase test_case = test_cases.elem[i];
        Scanner scanner           = init_scanner_from_str(test_case.source_str);
        Scanner *s                = &scanner;
        for (uint32_t j = 0; j < test_case.want_tokens.len; j++) {
            Token want_token = test_case.want_tokens.elem[j];
            Token got_token  = scan_token(s);
            if (!are_tokens_equal(want_token, got_token)) {
                print_str(warn_str);
                println_str(test_case.label);
                print_str(want_str);
                print_token(want_token);
                print_str(got_str);
                print_token(got_token);
                println();
            }
        }
    }
    return 0;
}