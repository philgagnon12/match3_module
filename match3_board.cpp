#include "match3_board.h"
#include "match3_cell.h"

void Match3Board::_notification(int p_what)
{

}


void
Match3Board::add_child_notify(Node *p_child)
{
	//print_line(vformat("%s", __FUNCTION__));
	Match3Cell* match3_cell = Object::cast_to<Match3Cell>(p_child);
    if( match3_cell )
    {
		//print_line(vformat("Added to Match3Board %d", match3_cell->get_category()));
	}
}

void
Match3Board::_bind_methods()
{

}

Match3Board::Match3Board()
{

}
