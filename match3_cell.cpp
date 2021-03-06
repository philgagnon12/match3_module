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

    static int xx = 0;

    switch (p_what) {
        case NOTIFICATION_DRAW:
            xx += 120;
            draw_rect( Rect2(xx,0,100,100), Color(255,0,0,255));
        break;
    }
}


void
Match3Cell::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("position_as", "column", "row"), &Match3Cell::position_as);
}

Match3Cell::Match3Cell()
{

}
