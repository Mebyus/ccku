#include <stdio.h>
#include <stdlib.h>

#include "fatal.h"
#include "parser.h"
#include "source.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        fatal(1, "not enough arguments");
    }

    char *path = argv[1];

    SourceReadResult read_result = read_source_from_file(path);
    if (read_result.erc != 0) {
        fatal(read_result.erc, "error reading file");
    }

    parse_source(read_result.source);
    return 0;
}
