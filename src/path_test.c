#include <stdio.h>
#include <stdlib.h>

#include "path.h"

typedef struct PathTestCase PathTestCase;

struct PathTestCase {
    u64 id;
    str label;
    str arg_dirty;
    str want_clean;
};

const u32 number_of_test_cases  = 19;
const PathTestCase test_cases[] = {
    {
        .id         = 1,
        .label      = STR("empty => dot"),
        .arg_dirty  = EMPTY_STR,
        .want_clean = STR("."),
    },
    {
        .id         = 2,
        .label      = STR("dot => dot"),
        .arg_dirty  = STR("."),
        .want_clean = STR("."),
    },
    {
        .id         = 3,
        .label      = STR("name => name"),
        .arg_dirty  = STR("a"),
        .want_clean = STR("a"),
    },
    {
        .id         = 4,
        .label      = STR("name => name"),
        .arg_dirty  = STR("abc"),
        .want_clean = STR("abc"),
    },
    {
        .id         = 5,
        .label      = STR("root => root"),
        .arg_dirty  = STR("/"),
        .want_clean = STR("/"),
    },
    {
        .id         = 6,
        .label      = STR("dot + slash => dot"),
        .arg_dirty  = STR("./"),
        .want_clean = STR("."),
    },
    {
        .id         = 7,
        .label      = STR("dot + slash + name => name"),
        .arg_dirty  = STR("./a"),
        .want_clean = STR("a"),
    },
    {
        .id         = 8,
        .label      = STR("dot + slash + name + slash => name"),
        .arg_dirty  = STR("./abc/"),
        .want_clean = STR("abc"),
    },
    {
        .id         = 9,
        .label      = STR("name + slash => name"),
        .arg_dirty  = STR("a/"),
        .want_clean = STR("a"),
    },
    {
        .id         = 10,
        .label      = STR("name + slash + name => name + slash + name"),
        .arg_dirty  = STR("a/b"),
        .want_clean = STR("a/b"),
    },
    {
        .id         = 11,
        .label      = STR("name + slash + name + slash => name + slash + name"),
        .arg_dirty  = STR("a/b/"),
        .want_clean = STR("a/b"),
    },
    {
        .id         = 12,
        .label      = STR("name + slash + slash + name => name + slash + name"),
        .arg_dirty  = STR("a//b"),
        .want_clean = STR("a/b"),
    },
    {
        .id         = 13,
        .label      = STR("name + slash + slash + slash + name => name + slash + name"),
        .arg_dirty  = STR("a///b"),
        .want_clean = STR("a/b"),
    },
    {
        .id         = 14,
        .label      = STR("name + slash + dot + slash + name => name + slash + name"),
        .arg_dirty  = STR("a/./b"),
        .want_clean = STR("a/b"),
    },
    {
        .id         = 15,
        .label      = STR("dotdot => dotdot"),
        .arg_dirty  = STR(".."),
        .want_clean = STR(".."),
    },
    {
        .id         = 16,
        .label      = STR("dot + slash + dotdot => dotdot"),
        .arg_dirty  = STR("./.."),
        .want_clean = STR(".."),
    },
    {
        .id         = 17,
        .label      = STR("name + slash + dotdot => dot"),
        .arg_dirty  = STR("a/.."),
        .want_clean = STR("."),
    },
    {
        .id         = 18,
        .label      = STR("dotdot + slash + name => dotdot + slash + name"),
        .arg_dirty  = STR("../a"),
        .want_clean = STR("../a"),
    },
    {
        .id         = 19,
        .label      = STR("name + slash + dotdot + slash + name => name"),
        .arg_dirty  = STR("a/../a"),
        .want_clean = STR("a"),
    },
};

const str pass_str  = STR("    path_test [ OK ]");
const str fail_str  = STR("[ FAILED ]");
const str case_str  = STR("Test case: ");
const str input_str = STR("Input: ");
const str want_str  = STR("Want:  ");
const str got_str   = STR("Got:   ");

void print_failed_test_case(PathTestCase test_case, str got_path) {
    str id_str = format_u64_as_decimal(test_case.id);

    println();
    println_str(fail_str);
    print_str(case_str);
    print_str(id_str);
    fwrite(" (", 1, 2, stdout);
    print_str(test_case.label);
    fwrite(")\n", 1, 2, stdout);
    print_str(input_str);
    println_str(test_case.arg_dirty);
    print_str(want_str);
    println_str(test_case.want_clean);
    print_str(got_str);
    println_str(got_path);

    free_str(id_str);
}

void print_test_passed_message() {
    println();
    println_str(pass_str);
    println();
}

bool run_test_case(PathTestCase test_case) {
    bool failed  = false;
    str got_path = new_clean_path_from_str(test_case.arg_dirty);
    if (!are_strs_equal(test_case.want_clean, got_path)) {
        failed = true;
        print_failed_test_case(test_case, got_path);
    }
    free_str(got_path);
    return failed;
}

int main() {
    u32 failed_test_cases = 0;
    for (u32 i = 0; i < number_of_test_cases; i++) {
        bool failed = run_test_case(test_cases[i]);
        if (failed) {
            failed_test_cases++;
        }
    }
    if (failed_test_cases > 0) {
        println();
        exit(1);
    }
    print_test_passed_message();
    return 0;
}