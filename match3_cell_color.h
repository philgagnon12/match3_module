#pragma once

#include "match3_cell.h"

#include "match3/match3.h"

class Match3CellColor : public Match3Cell {
    GDCLASS(Match3CellColor, Match3Cell);

    uint8_t category = m3_cell_color_red;

protected:
    static void _bind_methods();

public:

    virtual void set_category(uint8_t category);
    virtual uint8_t get_category(void) const;

    Match3CellColor();
};
