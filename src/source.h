#ifndef KU_SOURCE_H
#define KU_SOURCE_H

#include <stddef.h>

#include "str.h"

typedef struct SourceText SourceText;
typedef struct SourceReadResult SourceReadResult;
typedef enum SourceReadErrCode SourceReadErrCode;

struct SourceText {
    str text;

    str filename;
    str filepath;
};

enum SourceReadErrCode {
    srec_NotAnError,
    srec_OpenFailed,
    srec_StatFailed,
    srec_NonPositiveFileSize,
    srec_FileTooLarge,
    srec_ReadError,
    srec_InconsistentSize,
};

struct SourceReadResult {
    SourceText source;
    SourceReadErrCode erc;
};

SourceReadResult read_source_from_file(char *path);
SourceText new_source_from_str(str s);
void free_source(SourceText source);

#endif // KU_SOURCE_H
