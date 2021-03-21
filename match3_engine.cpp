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
    this->m3_cell_to_board_cell.clear();
    for(int i = 0; i < this->board->get_child_count(); i++ )
    {
        Node* child = this->board->get_child(i);
        child->queue_delete();
        //memdelete(child);
    }

}

Match3Cell* Match3Engine::add_board_cell_from_m3_cell( struct m3_cell* cell )
{
    Map<uint8_t,Match3Cell*>::Element* e = this->category_to_engine_cell.find( cell->category );
    if( e != NULL )
    {
        Match3Cell* engine_cell = e->get();

        Node* node_board_cell = engine_cell->duplicate();

        Match3Cell* board_cell = Object::cast_to<Match3Cell>(node_board_cell);

        if( board_cell != NULL )
        {
            board_cell->set_column(cell->column);
            board_cell->set_row(cell->row);

            this->map_mutex->lock();
            this->board_cell_to_m3_cell.insert(board_cell, cell);
            this->m3_cell_to_board_cell.insert(cell, board_cell);
            this->map_mutex->unlock();

            this->board->add_child(board_cell);
            board_cell->set_owner(this->board);

            return board_cell;
        }

    }
    return NULL;

}

// TODO changing a category of an engine_cell from editor should re-trigger _board_build
Array
Match3Engine::board_build(void)
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

    if( this->m3_board != NULL )
    {
        m3_board_destroy(this->m3_board);

    }

    m3_board_build( &this->m3_options, &this->m3_board);

    m3_board_rand( &this->m3_options, this->m3_board);

    // Anything below 5 will hang the engine
    if( colors_count >= 5 )
    {
        m3_board_shuffle( &this->m3_options, this->m3_board->right->bottom );
    }

    this->board_clear_children();

    Array board_cells = Array();

    struct m3_cell* m3_cell_current = this->m3_board;
    while( m3_cell_current != NULL )
    {
        Match3Cell* board_cell = this->add_board_cell_from_m3_cell(m3_cell_current);
        if( board_cell != NULL)
        {
            board_cells.push_back(board_cell);
        }
        
        m3_cell_current = m3_cell_current->next;
    } // while

    return board_cells;
}

void Match3Engine::_notification(int p_what)
{
    if (p_what == NOTIFICATION_READY)
    {
        print_line("ready!");
    }
    else if( p_what == NOTIFICATION_ENTER_TREE )
    {
        print_line("enter tree!");
        this->board = memnew(Match3Board);
        this->add_child(this->board);

        this->map_mutex = Mutex::create();
    }
    else if(p_what == NOTIFICATION_EXIT_TREE)
    {
        print_line("exit tree!");
        this->board_clear_children();
        this->category_to_engine_cell.clear();
        this->engine_cells.clear();

        this->remove_child(this->board);
        memdelete(this->board);
        m3_match_result_destroy( &this->match_result_for_match_clear );
        if( this->m3_board != NULL )
        {
            m3_board_destroy(this->m3_board);
        }
        memfree(this->m3_colors);
        memdelete(this->map_mutex);


    }
    else if(p_what == NOTIFICATION_PREDELETE )
    {
        print_line("pre delete!");

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

    struct m3_cell* m3_subject = this->node_to_m3_cell(subject);
    struct m3_cell* m3_target = this->node_to_m3_cell(target);

    if( m3_subject != NULL && m3_target != NULL )
    {
        // cant swap with walls , as longs as its color it will work
        m3_swap( &m3_subject, &m3_target );


        if( m3_subject != NULL && m3_target != NULL )
        {
            Match3Cell* engine_subject = Object::cast_to<Match3Cell>(subject);
            Match3Cell* engine_target  = Object::cast_to<Match3Cell>(target);

            engine_subject->set_column(m3_target->column);
            engine_subject->set_row(m3_target->row);

            engine_target->set_column(m3_subject->column);
            engine_target->set_row(m3_subject->row);

            this->map_mutex->lock();
            this->board_cell_to_m3_cell.erase(engine_subject);
            this->board_cell_to_m3_cell.erase(engine_target);

            this->m3_cell_to_board_cell.erase(m3_subject);
            this->m3_cell_to_board_cell.erase(m3_target);

            this->board_cell_to_m3_cell.insert(engine_subject, m3_target);
            this->board_cell_to_m3_cell.insert(engine_target, m3_subject);

            this->m3_cell_to_board_cell.insert(m3_target, engine_subject);
            this->m3_cell_to_board_cell.insert(m3_subject, engine_target);
            this->map_mutex->unlock();


            print_line(vformat("engine swapped subject %d target %d", m3_target->category, m3_subject->category));
            
            this->emit_signal("swapped", engine_subject, engine_target);
        }
    }


}

bool
Match3Engine::cell_are_neighbours(Node* subject, Node* target)
{
    ERR_FAIL_NULL_V(subject, false);
    ERR_FAIL_NULL_V(target, false);

    struct m3_cell* m3_subject = this->node_to_m3_cell(subject);
    struct m3_cell* m3_target = this->node_to_m3_cell(target);

    if(m3_cell_are_neighbours(m3_subject, m3_target))
    {
        return true;
    }
    return false;
}

struct m3_cell*
Match3Engine::node_to_m3_cell(Node* node)
{
    ERR_FAIL_NULL_V(node, NULL);

    Match3Cell* match3_cell = Object::cast_to<Match3Cell>(node);

    ERR_FAIL_COND_V_MSG(!match3_cell, NULL, "node_to_m3_cell only works on Match3Cell.");

    this->map_mutex->lock();
    Map<Match3Cell*,struct m3_cell*>::Element* m3_cell_e = this->board_cell_to_m3_cell.find(match3_cell);
    this->map_mutex->unlock();

    ERR_FAIL_NULL_V_MSG(m3_cell_e, NULL, "struct m3_cell not found by Match3Cell");

    return m3_cell_e->get();
}

Array
Match3Engine::match(void)
{
    struct m3_match_result match_result = M3_MATCH_RESULT_CONST;

    m3_match( &this->m3_options,
              this->m3_board,
              &match_result );

    Array matched = Array();
    this->map_mutex->lock();
    for(int i = 0; i < match_result.matched_count; i++)
    {
        Map<struct m3_cell*, Match3Cell*>::Element* board_cell_e = NULL;
        board_cell_e = this->m3_cell_to_board_cell.find((struct m3_cell*)match_result.matched[i]);
        ERR_FAIL_NULL_V(board_cell_e, matched);

        Match3Cell* board_cell = board_cell_e->get();
        ERR_FAIL_NULL_V(board_cell, matched);
        matched.push_back(board_cell);
    }
    this->map_mutex->unlock();

    return matched;
}

Array
Match3Engine::match_either_cell( Node* a,
                                 Node* b )
{
    Array matches = Array();

    ERR_FAIL_NULL_V(a, matches);
    ERR_FAIL_NULL_V(b, matches);

    
    Array matches_a = Array();
    Array matches_b = Array();

    matches.push_back(matches_a);
    matches.push_back(matches_b);

    const struct m3_cell* m3_cell_a = (const struct m3_cell*)this->node_to_m3_cell(a);
    const struct m3_cell* m3_cell_b = (const struct m3_cell*)this->node_to_m3_cell(b);

    struct m3_match_result match_result_a = M3_MATCH_RESULT_CONST;
    struct m3_match_result match_result_b = M3_MATCH_RESULT_CONST;

    m3_match_either_cell( &this->m3_options,
                          m3_cell_a,
                          m3_cell_b,
                          &match_result_a,
                          &match_result_b );

    this->map_mutex->lock();
    for(int i = 0; i < match_result_a.matched_count; i++)
    {
        Map<struct m3_cell*,Match3Cell*>::Element* e = this->m3_cell_to_board_cell.find((struct m3_cell*)match_result_a.matched[i]);
        Match3Cell* engine_cell_match = e->get();
        // TODO on fail im not m3_match_result_destroy
        ERR_FAIL_NULL_V(engine_cell_match, Array());
        matches_a.push_back(engine_cell_match);
    }
    for(int i = 0; i < match_result_b.matched_count; i++)
    {
        Map<struct m3_cell*,Match3Cell*>::Element* e = this->m3_cell_to_board_cell.find((struct m3_cell*)match_result_b.matched[i]);
        Match3Cell* engine_cell_match = e->get();
        // TODO on fail im not m3_match_result_destroy
        ERR_FAIL_NULL_V(engine_cell_match, Array());
        matches_b.push_back(engine_cell_match);
    }


    this->map_mutex->unlock();

    m3_match_result_destroy(&match_result_a);
    m3_match_result_destroy(&match_result_b);

    return matches;
}

void Match3Engine::match_clear( Array matches )
{
    if( matches.size() <= 0 )
        return;

    for(int i = 0; i < matches.size(); i++)
    {
       Match3Cell* engine_cell = Object::cast_to<Match3Cell>(matches[i]);
       ERR_FAIL_NULL(engine_cell);
       struct m3_cell* cell = this->node_to_m3_cell(Object::cast_to<Node>(engine_cell));
       ERR_FAIL_NULL(cell);
       
       if( i == 0 )
       {
            m3_match_result_init( &this->match_result_for_match_clear,
                                  cell );
       }
       else
       {
            m3_match_result_add_match( &this->match_result_for_match_clear,
                                       cell );
       }
    }

    m3_match_clear( &this->m3_options,
                    &this->match_result_for_match_clear );

    Array matches_cleared;

    this->map_mutex->lock();
    for(int i = 0; i < this->match_result_for_match_clear.matched_count; i++)
    {
        Map<struct m3_cell*,Match3Cell*>::Element* e = this->m3_cell_to_board_cell.find((struct m3_cell*)this->match_result_for_match_clear.matched[i]);
        ERR_FAIL_NULL(e);
        Match3Cell* board_cell = e->get();
        ERR_FAIL_NULL(board_cell);
        board_cell->set_category(this->match_result_for_match_clear.matched[i]->category);
        matches_cleared.push_back(board_cell);
    }
    this->map_mutex->unlock();

    this->_match_cleared(matches_cleared);
}

void Match3Engine::_match_cleared( Array matches_cleared )
{
    if (get_script_instance() && get_script_instance()->has_method("_match_cleared")) {

        get_script_instance()->call("_match_cleared", matches_cleared);
    }
}

// TODO this was kind of re-adapted from the m3 engine and not using the original function ....
// the term to_path is not in context anymore since i changed this func
Array
Match3Engine::match_clear_sort( Array matches_cleared )
{
    const struct m3_cell* cell_first_top_color = NULL;

    struct m3_cell* cell_to_fallthrough = NULL;

    Array board_cells_sorted = Array();

    this->map_mutex->lock();
    for(int i = 0; i < matches_cleared.size(); i++)
    {
        Match3Cell* engine_cell_matched = Object::cast_to<Match3Cell>(matches_cleared[i]);
        ERR_FAIL_NULL_V(engine_cell_matched, board_cells_sorted);
        struct m3_cell* cell_matched = this->node_to_m3_cell(Object::cast_to<Node>(engine_cell_matched));
        ERR_FAIL_NULL_V(cell_matched, board_cells_sorted);


        m3_cell_find_first_top_color( cell_matched, &cell_first_top_color );

        cell_to_fallthrough = (struct m3_cell*)cell_first_top_color;

        while( cell_to_fallthrough != NULL )
        {
            Map<struct m3_cell*, Match3Cell*>::Element* engine_cell_to_path_e = this->m3_cell_to_board_cell.find(cell_to_fallthrough);
            ERR_FAIL_NULL_V(engine_cell_to_path_e, board_cells_sorted);

            Match3Cell* engine_cell_to_path = engine_cell_to_path_e->get();
            ERR_FAIL_NULL_V(engine_cell_to_path_e, board_cells_sorted);

            this->board_cell_to_m3_cell.erase(engine_cell_to_path);
            this->m3_cell_to_board_cell.erase(cell_to_fallthrough);

            m3_cell_fallthrough( &this->m3_options, &cell_to_fallthrough );

            this->m3_cell_to_board_cell.insert(cell_to_fallthrough, engine_cell_to_path);
            this->board_cell_to_m3_cell.insert(engine_cell_to_path, cell_to_fallthrough);

            engine_cell_to_path->set_column(cell_to_fallthrough->column);
            engine_cell_to_path->set_row(cell_to_fallthrough->row);

            board_cells_sorted.push_back(engine_cell_to_path);

            m3_cell_find_first_top_color( cell_to_fallthrough, &cell_first_top_color );

            cell_to_fallthrough = (struct m3_cell*)cell_first_top_color;
        }
    } // for
    this->map_mutex->unlock();

    return board_cells_sorted;
    //this->_match_clear_sorted(matches_cleared);
    //matches_cleared.clear(); TODO find how the engine could clear those
}

void Match3Engine::_match_clear_sorted( Array matches_cleared )
{
    if (get_script_instance() && get_script_instance()->has_method("_match_clear_sorted")) {

        get_script_instance()->call("_match_clear_sorted", matches_cleared);
    }
}

Array
Match3Engine::board_fill(void)
{
    struct m3_cell* cell_current = this->m3_board;

    Array board_filled_cells;

    while( cell_current != NULL )
    {
        if( cell_current->category == ( m3_cell_flag_color | m3_cell_flag_color_open ) )
        {
            m3_cell_rand( &this->m3_options, cell_current );
            Match3Cell* added = this->add_board_cell_from_m3_cell(cell_current);
            ERR_FAIL_NULL_V(added, board_filled_cells);
            board_filled_cells.push_back(added);
        }
        cell_current = cell_current->next;
    }

    return board_filled_cells;
}

void Match3Engine::_board_filled( Array added_hidden_cells )
{
    if (get_script_instance() && get_script_instance()->has_method("_board_filled")) {

        get_script_instance()->call("_board_filled", added_hidden_cells);
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

    ClassDB::bind_method(D_METHOD("board_build"), &Match3Engine::board_build);

    ClassDB::bind_method(D_METHOD("swap", "subject", "target"), &Match3Engine::swap);
    ADD_SIGNAL(MethodInfo("swapped", PropertyInfo(Variant::OBJECT, "subject"), PropertyInfo(Variant::OBJECT, "target") ));

    ClassDB::bind_method(D_METHOD("cell_are_neighbours", "subject", "target"), &Match3Engine::cell_are_neighbours);
    ClassDB::bind_method(D_METHOD("match"), &Match3Engine::match);
    ClassDB::bind_method(D_METHOD("match_either_cell", "a", "b"), &Match3Engine::match_either_cell);

    ClassDB::bind_method(D_METHOD("match_clear", "matches"), &Match3Engine::match_clear);
    BIND_VMETHOD(MethodInfo("_match_cleared", PropertyInfo(Variant::ARRAY, "matches_cleared") ));

    ClassDB::bind_method(D_METHOD("match_clear_sort", "matches_cleared"), &Match3Engine::match_clear_sort);
    BIND_VMETHOD(MethodInfo("_match_clear_sorted", PropertyInfo(Variant::ARRAY, "matches_cleared") ));

    ClassDB::bind_method(D_METHOD("board_fill"), &Match3Engine::board_fill);
    BIND_VMETHOD(MethodInfo("_board_filled", PropertyInfo(Variant::ARRAY, "added_hidden_cells") ));

}

Match3Engine::Match3Engine()
{
    RandomNumberGenerator rng;
    rng.randomize();
    this->m3_options.seed = (int)rng.randi();


}

Match3Engine::~Match3Engine()
{

}
