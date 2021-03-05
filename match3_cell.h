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

    struct m3_cell* cell = NULL;
    Match3Cell();
};
