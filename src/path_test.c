#include "path.h"

typedef struct PathTestCase PathTestCase;

struct PathTestCase {
    u64 id;
    str label;
    str arg_dirty;
    str want_clean;
};

const PathTestCase test_cases[] = {
    {
        .id         = 1,
        .label      = STR("empty path => dot"),
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
        .label      = STR("relative name => the same name"),
        .arg_dirty  = STR("a"),
        .want_clean = STR("a"),
    },
    {
        .id         = 4,
        .label      = STR("relative name => the same name"),
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
        .label      = STR("dot + slash + name => relative name"),
        .arg_dirty  = STR("./a"),
        .want_clean = STR("a"),
    },
    {
        .arg_dirty  = STR("./a"),
        .want_clean = STR("a"),
    },
    {
        .arg_dirty  = STR("a/"),
        .want_clean = STR("a"),
    },
    {
        .arg_dirty  = STR("a/b"),
        .want_clean = STR("a/b"),
    },
    {
        .arg_dirty  = STR("a/b/"),
        .want_clean = STR("a/b"),
    },
    {
        .arg_dirty  = STR("a//b"),
        .want_clean = STR("a/b"),
    },
};

int main() {
    return 0;
}