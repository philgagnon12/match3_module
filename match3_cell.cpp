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
Match3Cell::_bind_methods()
{
    BIND_VMETHOD(MethodInfo("_position_as", PropertyInfo(Variant::INT, "column"), PropertyInfo(Variant::INT, "row") ));
}

Match3Cell::Match3Cell()
{

}
