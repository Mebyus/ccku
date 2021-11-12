#include "position.h"

Position init_position(u32 line, u32 column) {
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

bool are_positions_equal(Position pos1, Position pos2) {
    return (pos1.line == pos2.line) && (pos1.column == pos2.column);
}