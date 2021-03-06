#pragma once

#include "scene/2d/node_2d.h"

#include "match3/match3.h"


class Match3Board : public Node2D {
    GDCLASS(Match3Board, Node2D);



protected:
    static void _bind_methods();
    void _notification(int p_what);
    virtual void add_child_notify(Node *p_child);

public:

    Match3Board();
};
