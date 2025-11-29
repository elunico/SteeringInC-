#include "food.h"
#include <ncurses.h>

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
    mvprintw(static_cast<int>(position.y), static_cast<int>(position.x), "F");
}

void Food::markEaten()
{
    wasEaten = true;
}