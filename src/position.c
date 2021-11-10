#include "position.h"

Position init_position(uint32_t line, uint32_t column) {
    Position pos = {
        .line   = line,
        .column = column,
    };
    return pos;
}

Position init_null_position() {
    return init_position(1, 1);
}

Position next_line(Position pos) {
    pos.line++;
    pos.column = 1;
    return pos;
}

Position next_column(Position pos) {
    pos.column++;
    return pos;
}
