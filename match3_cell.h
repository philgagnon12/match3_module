#pragma once

#include "scene/2d/node_2d.h"

#include "match3/match3.h"
#include "match3/cell.h"

class Match3Cell : public Node2D {
    GDCLASS(Match3Cell, Node2D);

protected:
    static void _bind_methods();
    void _notification(int p_what);

public:

    virtual void set_category(uint8_t category);
    virtual uint8_t get_category(void) const;

    virtual void position_as( uint8_t column, uint8_t row){};

    Match3Cell();
};
