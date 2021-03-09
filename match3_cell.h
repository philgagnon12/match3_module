#pragma once

#include "scene/2d/node_2d.h"

#include "match3/match3.h"
#include "match3/cell.h"

class Match3Cell : public Node2D {
    GDCLASS(Match3Cell, Node2D);
    uint8_t column = 0;
    uint8_t row = 0;

protected:
    static void _bind_methods();
    void _notification(int p_what);

public:

    virtual void set_category(uint8_t category);
    virtual uint8_t get_category(void) const;

    void set_column(uint8_t column);
    uint8_t get_column(void) const;

    void set_row(uint8_t row);
    uint8_t get_row(void) const;

    // TODO get rid of these two
    virtual void _position_as( uint8_t column, uint8_t row);
    virtual void _path_to( uint8_t column, uint8_t row);

    Match3Cell();
};
