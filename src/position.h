#ifndef KU_POSITION_H
#define KU_POSITION_H

#include "types.h"

typedef struct Position Position;

struct Position {
    u32 line;
    u32 column;
};

extern const Position null_position;

Position init_position(u32 line, u32 column);
Position next_line(Position pos);
Position next_column(Position pos);
bool are_positions_equal(Position pos1, Position pos2);

#endif // KU_POSITION_H
