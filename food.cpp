#include "food.h"

void Food::update()
{
    if (lifespan > 0) {
        lifespan--;
    } else {
        wasEaten = true;
    }
}

void Food::mark_eaten()
{
    wasEaten = true;
}

Vec2D const& Food::get_position() const
{
    return position;
}