#ifndef KU_PATH_H
#define KU_PATH_H

#include "str.h"
#include "types.h"

typedef struct Path Path;

struct Path {
    bool is_abs;
    str base;
    str ext;
    str dir;
    str path;
};

str new_clean_path_from_bytes(const byte *bytes, u64 size);
str new_clean_path_from_cstr(const char *cstr);
str new_clean_path_from_str(str s);
Path borrow_path_from_str(str dirty_path);
Path take_path_from_cstr(char* cstr);
void free_path(Path path);

#endif // KU_PATH_H
