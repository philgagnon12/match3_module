#pragma once

#include "match3_cell.h"

#include "scene/main/node.h"
#include "core/object.h"

#include "match3/match3.h"
#include "match3/cell.h"
#include "match3/board.h"

class Match3Board : public Node {
    GDCLASS(Match3Board, Node);

    struct m3_options options = M3_OPTIONS_CONST;

    List<Match3Cell*> base_cells;

    struct m3_cell* board = NULL;

    uint8_t* colors = NULL;
    size_t colors_size = 0;


protected:
    void _board_build(void);

    static void _bind_methods();
    virtual void add_child_notify(Node *p_child);
    virtual void remove_child_notify(Node *p_child);
    void _notification(int p_what);

public:
    void set_seed(int seed);
    int get_seed(void) const;

    void set_columns(int columns);
    int get_columns(void) const;

    void set_rows(int rows);
    int get_rows(void) const;

    void set_matches_required_to_clear(int matches_required_to_clear);
    int get_matches_required_to_clear(void) const;


    Match3Board();
};
