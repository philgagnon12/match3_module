#include "match3_engine.h"

#include "core/math/random_number_generator.h"

void
Match3Engine::set_seed(int seed)
{
    this->m3_options.seed = seed;
}

int
Match3Engine::get_seed(void) const
{
    return this->m3_options.seed;
}

void
Match3Engine::set_columns(int columns)
{
    this->m3_options.columns = columns;
}

int
Match3Engine::get_columns(void) const
{
    return this->m3_options.columns;
}

void
Match3Engine::set_rows(int rows)
{
    this->m3_options.rows = rows;
}

int
Match3Engine::get_rows(void) const
{
    return this->m3_options.rows;
}

void
Match3Engine::set_matches_required_to_clear(int matches_required_to_clear)
{
    this->m3_options.matches_required_to_clear = matches_required_to_clear;
}

int
Match3Engine::get_matches_required_to_clear(void) const
{
    return this->m3_options.matches_required_to_clear;
}

void
Match3Engine::board_clear_children()
{

    this->board_cell_to_m3_cell.clear();
    for(int i = 0; i < this->board->get_child_count(); i++ )
    {
        Node* child = this->board->get_child(i);
        this->board->remove_child(child);
    }

}

// TODO changing a category of an engine_cell from editor should re-trigger _board_build
void
Match3Engine::_board_build(void)
{
    uint8_t* colors_re = NULL;

    size_t colors_size = 0;

    uint8_t colors_count = 0;

    // Reset colors array
    for(List<Match3Cell*>::Element* e = this->engine_cells.front(); e; e=e->next())
    {
        colors_size = sizeof( uint8_t ) * (++colors_count);

        if( colors_size > this->m3_colors_size )
        {
            colors_re = (uint8_t*)memrealloc( this->m3_colors, colors_size);
            CRASH_COND( colors_re == NULL );

            this->m3_colors = colors_re;
            this->m3_colors_size = colors_size;

        }

        Match3Cell* match3_cell = e->get();

        this->m3_colors[colors_count-1] = match3_cell->get_category();
        print_line(vformat("m3_colors %d %02X", (int)this->m3_colors[colors_count-1], this->m3_colors[colors_count-1]));
    }

    // TODO better error message
    // ERR_FAIL_COND_MSG( colors_count < this->options.matches_required_to_clear, "You should have lots of colors");

    this->m3_options.colors = this->m3_colors;
    this->m3_options.colors_size = this->m3_colors_size;

    // TODO re-enable
    // if( this->m3_board != NULL )
    // {
    //     board_destroy(this->m3_board);
    // }

    board_build( &this->m3_options, &this->m3_board);

    board_rand( &this->m3_options, this->m3_board);

    // Anything below 5 will hang the engine
    if( colors_count >= 5 )
    {
        board_shuffle( &this->m3_options, this->m3_board->right->bottom );
    }

    this->board_clear_children();

    struct m3_cell* m3_cell_current = this->m3_board;
    while( m3_cell_current != NULL )
    {
        Map<uint8_t,Match3Cell*>::Element* e = this->category_to_engine_cell.find( m3_cell_current->category );
        if( e != NULL )
        {
            Match3Cell* engine_cell = e->get();

            Node* node_board_cell = engine_cell->duplicate();

            Match3Cell* board_cell = Object::cast_to<Match3Cell>(node_board_cell);

            if( board_cell != NULL )
            {
                this->board_cell_to_m3_cell.insert(board_cell, m3_cell_current);

                this->board->add_child(board_cell);
                board_cell->set_owner(this->board);

                board_cell->_position_as(m3_cell_current->column, m3_cell_current->row);

            }

        }
        m3_cell_current = m3_cell_current->next;
    } // while

}

void Match3Engine::_notification(int p_what)
{
    if (p_what == NOTIFICATION_READY)
    {
        print_line("ready!");
        this->_board_build();
    }
}


void
Match3Engine::add_child_notify(Node *p_child)
{
    Match3Cell* match3_cell = Object::cast_to<Match3Cell>(p_child);
    if( match3_cell )
    {
        this->engine_cells.push_back(match3_cell);
        this->category_to_engine_cell.insert( match3_cell->get_category(), match3_cell );

    }
}

void
Match3Engine::remove_child_notify(Node *p_child)
{
    Match3Cell* match3_cell = Object::cast_to<Match3Cell>(p_child);
    if( match3_cell )
    {
        this->engine_cells.erase(match3_cell);
        this->category_to_engine_cell.erase( match3_cell->get_category() );
        // this->_board_build();
    }
}

void
Match3Engine::swap(Node* subject, Node* target)
{
    ERR_FAIL_NULL(subject);
    ERR_FAIL_NULL(target);

    Match3Cell* match3_cell_subject = Object::cast_to<Match3Cell>(subject);
    Match3Cell* match3_cell_target  = Object::cast_to<Match3Cell>(target);

    ERR_FAIL_COND_MSG(!match3_cell_subject, "swap subject only works on Match3Cell.");
    ERR_FAIL_COND_MSG(!match3_cell_target,  "swap target only works on Match3Cell.");

    Map<Match3Cell*,struct m3_cell*>::Element* m3_subject_e = this->board_cell_to_m3_cell.find(match3_cell_subject);
    Map<Match3Cell*,struct m3_cell*>::Element* m3_target_e = this->board_cell_to_m3_cell.find(match3_cell_target);

    struct m3_cell* m3_subject = m3_subject_e->get();
    struct m3_cell* m3_target = m3_target_e->get();

    if( m3_subject != NULL && m3_target != NULL )
    {
        // cant swap with walls , as longs as its color it will work
        m3_swap( &m3_subject, &m3_target );


        if( m3_subject != NULL && m3_target != NULL )
        {
            print_line(vformat("engine swapped subject %d target %d", match3_cell_subject->get_category(), match3_cell_target->get_category()));
            // swap worked
            this->_swapped( match3_cell_subject, match3_cell_target );
        }
    }


}

void
Match3Engine::_swapped(Match3Cell* subject, Match3Cell* target)
{
    if (get_script_instance() && get_script_instance()->has_method("_swapped")) {

        get_script_instance()->call("_swapped", subject, target);
    }
}


void
Match3Engine::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_seed", "seed"), &Match3Engine::set_seed);
    ClassDB::bind_method(D_METHOD("get_seed"), &Match3Engine::get_seed);

    ClassDB::bind_method(D_METHOD("set_columns", "columns"), &Match3Engine::set_columns);
    ClassDB::bind_method(D_METHOD("get_columns"), &Match3Engine::get_columns);

    ClassDB::bind_method(D_METHOD("set_rows", "rows"), &Match3Engine::set_rows);
    ClassDB::bind_method(D_METHOD("get_rows"), &Match3Engine::get_rows);

    ClassDB::bind_method(D_METHOD("set_matches_required_to_clear", "matches_required_to_clear"), &Match3Engine::set_matches_required_to_clear);
    ClassDB::bind_method(D_METHOD("get_matches_required_to_clear"), &Match3Engine::get_matches_required_to_clear);



    ADD_PROPERTY(PropertyInfo(Variant::INT, "seed"), "set_seed", "get_seed");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "columns"), "set_columns", "get_columns");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "rows"), "set_rows", "get_rows");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "matches_required_to_clear"), "set_matches_required_to_clear", "get_matches_required_to_clear");

    ClassDB::bind_method(D_METHOD("swap", "subject", "target"), &Match3Engine::swap);
    BIND_VMETHOD(MethodInfo("_swapped", PropertyInfo(Variant::OBJECT, "subject"), PropertyInfo(Variant::OBJECT, "target") ));

}

Match3Engine::Match3Engine()
{
    RandomNumberGenerator rng;
    rng.randomize();
    this->m3_options.seed = (int)rng.randi();

    this->board = memnew(Match3Board);
    this->add_child(this->board);
}

Match3Engine::~Match3Engine()
{
    // print_line(vformat("%s", __FUNCTION__));
    // this->board_clear_children();
    // this->category_to_engine_cell.clear();
    // this->engine_cells.clear();
    // if( this->m3_board != NULL )
    // {
    //     board_destroy(this->m3_board);
    // }
    // memfree(this->m3_colors);
}
