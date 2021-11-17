#ifndef KU_STROP_H
#define KU_STROP_H

#include "types.h"
#include "str.h"
#include "slice.h"

TYPEDEF_SLICE(str)

slice_of_strs borrow_split_str_by_byte(str s, byte b);

#endif // KU_STROP_H
