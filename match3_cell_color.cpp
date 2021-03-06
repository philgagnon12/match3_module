#include "match3_cell_color.h"

void
Match3CellColor::set_category(uint8_t category)
{
    this->category = ( m3_cell_flag_color | category ) & m3_cell_mask_color;
}

uint8_t
Match3CellColor::get_category(void) const
{
    return ( m3_cell_flag_color | this->category ) & m3_cell_mask_color;
}

void
Match3CellColor::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_category", "category"), &Match3CellColor::set_category);
    ClassDB::bind_method(D_METHOD("get_category"), &Match3CellColor::get_category);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "category", PROPERTY_HINT_RANGE, vformat("1,%d,1", m3_cell_mask_color)), "set_category", "get_category");
}

Match3CellColor::Match3CellColor()
{

}
