#include "food.h"

void Food::update()
{
    if (lifespan > 0) {
        lifespan--;
    } else {
        was_eaten = true;
    }
}

void Food::mark_eaten()
{
    was_eaten = true;
}

Vec2D const& Food::get_position() const
{
    return position;
}