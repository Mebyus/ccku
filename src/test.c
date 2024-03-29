#include "scanner.h"
#include "slice.h"
#include "split_test_scanner.h"
#include "strop.h"
#include "types.h"
#include <stdio.h>

TYPEDEF_SLICE(Token);
IMPLEMENT_SLICE(Token);

typedef struct ScannerTestCase ScannerTestCase;
typedef struct ScannerTestSuite ScannerTestSuite;

struct ScannerTestCase {
    str id;
    str label;
    str source_str;
    slice_of_Tokens want_tokens;
};

TYPEDEF_SLICE(ScannerTestCase);
IMPLEMENT_SLICE(ScannerTestCase);

struct ScannerTestSuite {
    SourceText source;
    slice_of_ScannerTestCases cases;
};

const str id_control_line      = STR("## id:");
const str label_control_line   = STR("## label:");
const str program_control_line = STR("## program:");
const str tokens_control_line  = STR("## tokens:");
const str end_control_line     = STR("## end");


const str case_str = STR("Test case: ");
const str want_str = STR("Want: ");
const str got_str  = STR("Got:  ");

u32 run_test_cases(slice_of_ScannerTestCases test_cases) {
    u32 failed = 0;
    for (u32 i = 0; i < test_cases.len; i++) {
        ScannerTestCase test_case = test_cases.elem[i];
        Scanner scanner           = init_scanner_from_str(test_case.source_str);
        Scanner *s                = &scanner;
        for (u32 j = 0; j < test_case.want_tokens.len; j++) {
            Token want_token = test_case.want_tokens.elem[j];
            Token got_token  = scan_token(s);
            if (!are_tokens_equal(want_token, got_token)) {
                failed++;
                println();
                print_str(case_str);
                print_str(test_case.id);
                fwrite(" (", 1, 2, stdout);
                print_str(test_case.label);
                fwrite(")\n", 1, 2, stdout);
                print_str(want_str);
                print_token(want_token);
                print_str(got_str);
                print_token(got_token);
            }
            free_token(got_token);
        }
    }
    return failed;
}

int run_test_suite(ScannerTestSuite suite) {
    u32 failed_test_cases = run_test_cases(suite.cases);
    if (failed_test_cases > 0) {
        return 1;
    }
    return 0;
}

void cleanup_test_cases(slice_of_ScannerTestCases test_cases) {
    for (u32 i = 0; i < test_cases.len; i++) {
        ScannerTestCase test_case = test_cases.elem[i];
        free_slice_of_Tokens(test_case.want_tokens);
    }
    free_slice_of_ScannerTestCases(test_cases);
}

void cleanup_test_suite(ScannerTestSuite suite) {
    cleanup_test_cases(suite.cases);
    free_source(suite.source);
}

slice_of_Tokens parse_test_tokens(str text) {
    slice_of_strs split    = borrow_split_str_by_byte(text, '\n');
    slice_of_Tokens tokens = empty_slice_of_Tokens;
    for (u32 i = 0; i < split.len; i++) {
        TokenParseResult res = parse_token_from_str(split.elem[i]);
        if (res.ok) {
            append_Token_to_slice(&tokens, res.token);
        }
    }
    free_slice_of_strs(split);
    return tokens;
}

ScannerTestSuite create_test_suite_from_source(SourceText source) {
    slice_of_ScannerTestCases test_cases = empty_slice_of_ScannerTestCases;
    ScannerTestCase test_case            = {
        .want_tokens = empty_slice_of_Tokens,
    };
    SplitTestScanner split_scanner = init_split_test_scanner(source.text);
    bool wait_for_program          = false;
    bool wait_for_tokens           = false;
    while (scan_next_split_test(&split_scanner)) {
        str text = split_scanner.next;
        if (split_scanner.control) {
            if (are_strs_equal(program_control_line, text)) {
                wait_for_program = true;
                wait_for_tokens  = false;
            } else if (are_strs_equal(tokens_control_line, text)) {
                wait_for_program = false;
                wait_for_tokens  = true;
            } else if (are_strs_equal(end_control_line, text)) {
                wait_for_program = false;
                wait_for_tokens  = false;
                append_ScannerTestCase_to_slice(&test_cases, test_case);
                test_case.want_tokens = empty_slice_of_Tokens;
            } else if (has_prefix_str(text, id_control_line)) {
                test_case.id = borrow_str_slice_to_end(text, id_control_line.len + 1);
            } else if (has_prefix_str(text, label_control_line)) {
                test_case.label = borrow_str_slice_to_end(text, label_control_line.len + 1);
            }
        } else {
            if (wait_for_program) {
                test_case.source_str = text;
            }
            if (wait_for_tokens) {
                test_case.want_tokens = parse_test_tokens(text);
            }
        }
    }

    ScannerTestSuite suite = {
        .source = source,
        .cases  = test_cases,
    };
    return suite;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fatal(1, "not enough arguments");
    }

    init_token_module();

    char *path                   = argv[1];
    SourceReadResult read_result = read_source_from_file(path);
    if (read_result.erc != 0) {
        fatal(read_result.erc, "error reading file");
    }

    ScannerTestSuite test_suite = create_test_suite_from_source(read_result.source);
    int code                    = run_test_suite(test_suite);
    cleanup_test_suite(test_suite);

    exit(code);
}