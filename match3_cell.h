#pragma once

#include "scene/main/node.h"
#include "match3/match3.h"
#include "match3/cell.h"

class Match3Cell : public Node {
    GDCLASS(Match3Cell, Node);

    struct m3_cell* cell = NULL;


protected:
    static void _bind_methods();

public:

    Match3Cell();
};
