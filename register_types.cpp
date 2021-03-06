
#include "register_types.h"

#include "core/class_db.h"

#include "match3_engine.h"
#include "match3_board.h"
#include "match3_cell.h"
#include "match3_cell_color.h"

void register_match3_module_types()
{
	ClassDB::register_class<Match3Engine>();
	ClassDB::register_class<Match3Board>();
	ClassDB::register_class<Match3Cell>();
	ClassDB::register_class<Match3CellColor>();

}

void unregister_match3_module_types()
{

}
