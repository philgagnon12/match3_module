#include "match3_board.h"

#include "core/math/random_number_generator.h"

void
Match3Board::set_seed(int seed)
{
    this->options.seed = seed;
}

int
Match3Board::get_seed(void) const
{
    return this->options.seed;
}

void
Match3Board::set_columns(int columns)
{
    this->options.columns = columns;
}

int
Match3Board::get_columns(void) const
{
    return this->options.columns;
}

void
Match3Board::set_rows(int rows)
{
    this->options.rows = rows;
}

int
Match3Board::get_rows(void) const
{
    return this->options.rows;
}

void
Match3Board::set_matches_required_to_clear(int matches_required_to_clear)
{
    this->options.matches_required_to_clear = matches_required_to_clear;
}

int
Match3Board::get_matches_required_to_clear(void) const
{
    return this->options.matches_required_to_clear;
}


void
Match3Board::_board_build(void)
{
    uint8_t* colors_re = NULL;

    size_t colors_size = 0;
    
    uint8_t colors_count = 0;

    for(List<Match3Cell*>::Element* e = this->base_cells.front(); e; e=e->next())
    {
        colors_size = sizeof( uint8_t ) * (++colors_count);

        if( colors_size > this->colors_size )
        {
            colors_re = (uint8_t*)memrealloc( this->colors, colors_size);
            CRASH_COND( colors_re == NULL );

            this->colors = colors_re;
            this->colors_size = colors_size;

        }
        
        this->colors[colors_count-1] = ( m3_cell_flag_color | colors_count ) & m3_cell_mask_color;
    }

    // TODO better error message
    ERR_FAIL_COND_MSG( colors_count < this->options.matches_required_to_clear, "You should have lots of colors");

    this->options.colors = this->colors;
    this->options.colors_size = this->colors_size;

    if( this->board != NULL )
    {
        board_destroy(this->board);
    }

    board_build( &this->options, &this->board);

    // board_rand( &this->options, this->board);

    // board_shuffle( &this->options, this->board->right->bottom );
}

void Match3Board::_notification(int p_what)
{
    if (p_what == NOTIFICATION_READY)
    {
        print_line("ready!");
    }
}


void
Match3Board::add_child_notify(Node *p_child)
{
    Match3Cell* match3_cell = Object::cast_to<Match3Cell>(p_child);
    if( match3_cell )
    {
        this->base_cells.push_back( match3_cell );
        this->_board_build();
    }
}

void
Match3Board::remove_child_notify(Node *p_child)
{
    Match3Cell* match3_cell = Object::cast_to<Match3Cell>(p_child);
    if( match3_cell )
    {
        this->base_cells.erase( match3_cell );
        this->_board_build();
    }
}


void
Match3Board::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_seed", "seed"), &Match3Board::set_seed);
    ClassDB::bind_method(D_METHOD("get_seed"), &Match3Board::get_seed);

    ClassDB::bind_method(D_METHOD("set_columns", "columns"), &Match3Board::set_columns);
    ClassDB::bind_method(D_METHOD("get_columns"), &Match3Board::get_columns);

    ClassDB::bind_method(D_METHOD("set_rows", "rows"), &Match3Board::set_rows);
    ClassDB::bind_method(D_METHOD("get_rows"), &Match3Board::get_rows);

    ClassDB::bind_method(D_METHOD("set_matches_required_to_clear", "matches_required_to_clear"), &Match3Board::set_matches_required_to_clear);
    ClassDB::bind_method(D_METHOD("get_matches_required_to_clear"), &Match3Board::get_matches_required_to_clear);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "seed"), "set_seed", "get_seed");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "columns"), "set_columns", "get_columns");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "rows"), "set_rows", "get_rows");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "matches_required_to_clear"), "set_matches_required_to_clear", "get_matches_required_to_clear");

}

Match3Board::Match3Board()
{
    RandomNumberGenerator rng;
    rng.randomize();
    this->options.seed = (int)rng.randi();
}
