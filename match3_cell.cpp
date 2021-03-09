#include "match3_cell.h"

void
Match3Cell::set_category(uint8_t category)
{

}

uint8_t
Match3Cell::get_category(void) const
{
    return 0;
}

void
Match3Cell::set_column(uint8_t column)
{
    this->column = column;
}

uint8_t
Match3Cell::get_column(void) const
{
    return this->column;
}

void
Match3Cell::set_row(uint8_t row)
{
    this->row = row;
}

uint8_t
Match3Cell::get_row(void) const
{
    return this->row;
}

void Match3Cell::_notification(int p_what) {

    // static int xx = 0;

    // switch (p_what) {
    //     case NOTIFICATION_DRAW:
    //         xx += 120;
    //         draw_rect( Rect2(xx,0,100,100), Color(255,0,0,255));
    //     break;
    // }
}

void
Match3Cell::_position_as( uint8_t column, uint8_t row)
{
	if (get_script_instance() && get_script_instance()->has_method("_position_as")) {

		get_script_instance()->call("_position_as", column, row);
	}
}

void
Match3Cell::_path_to( uint8_t column, uint8_t row)
{
    if (get_script_instance() && get_script_instance()->has_method("_path_to")) {

        get_script_instance()->call("_path_to", column, row);
    }
}

void
Match3Cell::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_column"), &Match3Cell::get_column);
    ClassDB::bind_method(D_METHOD("set_column", "column"), &Match3Cell::set_column);

    ClassDB::bind_method(D_METHOD("get_row"), &Match3Cell::get_row);
    ClassDB::bind_method(D_METHOD("set_row", "row"), &Match3Cell::set_row);
    
    ADD_PROPERTY(PropertyInfo(Variant::INT, "column"), "set_column", "get_column");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "row"), "set_row", "get_row");

    BIND_VMETHOD(MethodInfo("_position_as", PropertyInfo(Variant::INT, "column"), PropertyInfo(Variant::INT, "row") ));
    BIND_VMETHOD(MethodInfo("_path_to", PropertyInfo(Variant::INT, "column"), PropertyInfo(Variant::INT, "row") ));
}

Match3Cell::Match3Cell()
{

}
