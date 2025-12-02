#ifndef FOOD_H
#define FOOD_H

#include "utils.h"
#include "vec2d.h"

struct Food {
    Vec2D position;
    // prevent the initial food from all disappearing at once
    int  lifespan = randomInRange(850, 1750);  // lifespan in simulation ticks
    bool wasEaten = false;

    Vec2D const& get_position() const;

    void update();

    void mark_eaten();
};

#endif  // FOOD_H