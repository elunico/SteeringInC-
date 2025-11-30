#include "food.h"

void Food::update()
{
    if (lifespan > 0) {
        lifespan--;
    } else {
        wasEaten = true;
    }
}

void Food::markEaten()
{
    wasEaten = true;
}

Vec2D const& Food::getPosition() const
{
    return position;
}