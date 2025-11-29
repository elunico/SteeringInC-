#include "food.h"

void Food::update()
{
    if (lifespan > 0) {
        lifespan--;
    } else {
        wasEaten = true;
    }
}

void Food::show() const
{
}

void Food::markEaten()
{
    wasEaten = true;
}