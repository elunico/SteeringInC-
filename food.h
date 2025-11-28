#ifndef FOOD_H
#define FOOD_H

#include "vec2d.h"

struct Food {
    Vec2D position;
    int   lifespan = 1000;  // lifespan in simulation ticks
    bool  wasEaten = false;

    void update();

    void show() const;

    void markEaten();
};

#endif  // FOOD_H