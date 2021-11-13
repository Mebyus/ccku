#include "scanner.h"
#include "slice.h"
#include "split_test_scanner.h"
#include "types.h"

TYPEDEF_SLICE(Token);
IMPLEMENT_SLICE(Token);

typedef struct ScannerTestCase ScannerTestCase;

struct ScannerTestCase {
    u64 id;
    str label;
    str source_str;
    slice_of_Tokens want_tokens;
};

TYPEDEF_SLICE(ScannerTestCase);
IMPLEMENT_SLICE(ScannerTestCase);

const str program_control_line = STR("## program:");
const str tokens_control_line  = STR("## tokens:");
const str end_control_line     = STR("## end");


const str warn_str = STR("Tokens do not match in test case: ");
const str want_str = STR("Want: ");
const str got_str  = STR("Got:  ");

void run_test_cases(slice_of_ScannerTestCases test_cases) {
    println();
    for (u32 i = 0; i < test_cases.len; i++) {
        ScannerTestCase test_case = test_cases.elem[i];
        Scanner scanner           = init_scanner_from_str(test_case.source_str);
        Scanner *s                = &scanner;
        for (u32 j = 0; j < test_case.want_tokens.len; j++) {
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
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fatal(1, "not enough arguments");
    }

    char *path                   = argv[1];
    SourceReadResult read_result = read_source_from_file(path);
    if (read_result.erc != 0) {
        fatal(read_result.erc, "error reading file");
    }

    slice_of_ScannerTestCases test_cases = init_empty_slice_of_ScannerTestCases();
    ScannerTestCase test_case            = {
        .want_tokens = init_empty_slice_of_Tokens(),
    };
    SplitTestScanner split_scanner = init_split_test_scanner(read_result.source.text);
    bool wait_for_program          = false;
    while (scan_next_split_test(&split_scanner)) {
        str text = split_scanner.next;
        if (split_scanner.control) {
            if (are_strs_equal(program_control_line, text)) {
                wait_for_program = true;
            } else if (are_strs_equal(tokens_control_line, text)) {
                wait_for_program = false;
            } else if (are_strs_equal(end_control_line, text)) {
                append_ScannerTestCase_to_slice(&test_cases, test_case);
                test_case.want_tokens = init_empty_slice_of_Tokens();
            }
        } else {
            if (wait_for_program) {
                test_case.source_str = text;
            } else {
                
            }
        }
    }
    run_test_cases(test_cases);
    return 0;
}