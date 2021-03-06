#pragma once

#include "match3_cell.h"
#include "match3_board.h"

#include "scene/main/node.h"
#include "core/object.h"

#include "match3/match3.h"
#include "match3/cell.h"
#include "match3/board.h"
#include "match3/print.h"

class Match3Engine : public Node {
    GDCLASS(Match3Engine, Node);

    struct m3_options m3_options = M3_OPTIONS_CONST;

    List<Match3Cell*> engine_cells;

    Map<uint8_t,Match3Cell*> category_to_engine_cell;
    Map<Match3Cell*,uint8_t> engine_cell_to_category;

    struct m3_cell* m3_board = NULL;

    uint8_t* m3_colors = NULL;
    size_t m3_colors_size = 0;

    Match3Board* board = NULL;
    Map<Match3Cell*,struct m3_cell*> board_cell_to_m3_cell;

    // m3_cell_to_cell
    // cell_to_m3_cell

    void board_clear_children();

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

    Match3Engine();
    ~Match3Engine();
};
