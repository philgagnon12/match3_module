#include "match3_engine.h"

#include "core/math/random_number_generator.h"

void
Match3Engine::set_seed(int seed)
{
    this->options.seed = seed;
}

int
Match3Engine::get_seed(void) const
{
    return this->options.seed;
}

void
Match3Engine::set_columns(int columns)
{
    this->options.columns = columns;
}

int
Match3Engine::get_columns(void) const
{
    return this->options.columns;
}

void
Match3Engine::set_rows(int rows)
{
    this->options.rows = rows;
}

int
Match3Engine::get_rows(void) const
{
    return this->options.rows;
}

void
Match3Engine::set_matches_required_to_clear(int matches_required_to_clear)
{
    this->options.matches_required_to_clear = matches_required_to_clear;
}

int
Match3Engine::get_matches_required_to_clear(void) const
{
    return this->options.matches_required_to_clear;
}

void
Match3Engine::_board_build(void)
{
    uint8_t* colors_re = NULL;

    size_t colors_size = 0;

    uint8_t colors_count = 0;

    this->colors_to_base_cells.clear();

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
        this->colors_to_base_cells.insert( this->colors[colors_count-1], e->get());
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

    board_rand( &this->options, this->board);

    board_shuffle( &this->options, this->board->right->bottom );

    struct m3_cell* cell_current = this->board;
    while( cell_current != NULL )
    {
        Map<uint8_t,Match3Cell*>::Element* e = this->colors_to_base_cells.find( cell_current->category );
        if( e != NULL )
        {
            Match3Cell* base_cell_for_color = e->get();
            Node* node_cell_for_color = base_cell_for_color->duplicate();
            Match3Cell* cell_for_color = Object::cast_to<Match3Cell>(node_cell_for_color);
            if( cell_for_color != NULL )
            {
                cell_for_color->cell = cell_current;
                Node* memNode = memnew(Node);
                this->add_child(memNode);
                printf("asd\n");
            }

        }
        cell_current = cell_current->next;
    }

    print_tree();

}

void Match3Engine::_notification(int p_what)
{
    if (p_what == NOTIFICATION_READY)
    {
        print_line("ready!");
    }
}


void
Match3Engine::add_child_notify(Node *p_child)
{
    Match3Cell* match3_cell = Object::cast_to<Match3Cell>(p_child);
    if( match3_cell )
    {
        this->base_cells.push_back( match3_cell );
        this->_board_build();
    }
}

void
Match3Engine::remove_child_notify(Node *p_child)
{
    Match3Cell* match3_cell = Object::cast_to<Match3Cell>(p_child);
    if( match3_cell )
    {
        this->base_cells.erase( match3_cell );
        this->_board_build();
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

}

Match3Engine::Match3Engine()
{
    RandomNumberGenerator rng;
    rng.randomize();
    this->options.seed = (int)rng.randi();
}
