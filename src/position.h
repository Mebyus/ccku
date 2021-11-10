#ifndef KU_POSITION_H
#define KU_POSITION_H

#include <stdbool.h>
#include <stdint.h>

typedef struct Position Position;

struct Position {
    uint32_t line;
    uint32_t column;
};

Position init_null_position();
Position init_position(uint32_t line, uint32_t column);
Position next_line(Position pos);
Position next_column(Position pos);

#endif // KU_POSITION_H
