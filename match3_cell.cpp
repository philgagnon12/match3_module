#include "match3_cell.h"

void Match3Cell::_notification(int p_what) {

    static int xx = 0;
    xx += 20;
    switch (p_what) {
        case NOTIFICATION_DRAW:
            draw_rect( Rect2(xx,0,100,100), Color(255,0,0,255));
        break;
    }
}


void
Match3Cell::_bind_methods()
{

}

Match3Cell::Match3Cell()
{

}
