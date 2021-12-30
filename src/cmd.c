#include <stdlib.h>

#include "fatal.h"
#include "parser.h"
#include "source.h"

const str scan_cmd_name  = STR("scan");
const str parse_cmd_name = STR("parse");

void execute_scan_cmd(char *path) {
    SourceReadResult read_result = read_source_from_file(path);
    if (read_result.erc != 0) {
        fatal(read_result.erc, "error reading file");
    }

    Scanner *scanner = new_scanner_from_source(read_result.source);

    Token token;
    do {
        token = scan_token(scanner);
        print_token(token);
    } while (token.type != tt_EOF);
}

void execute_parse_cmd(char *path) {
    SourceReadResult read_result = read_source_from_file(path);
    if (read_result.erc != 0) {
        fatal(read_result.erc, "error reading file");
    }
    parse_standalone_source_from_str(read_result.source.text);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fatal(1, "not enough arguments");
    }

    init_token_module();

    str cmd_str = take_str_from_cstr(argv[1]);
    char *path  = argv[2];

    if (are_strs_equal(scan_cmd_name, cmd_str)) {
        execute_scan_cmd(path);
    } else if (are_strs_equal(parse_cmd_name, cmd_str)) {
        execute_parse_cmd(path);
    } else {
        fatal(1, "unknown command");
    }
    return 0;
}
