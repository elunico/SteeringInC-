#ifndef FOOD_H
#define FOOD_H

#include "utils.h"
#include "vec2d.h"

class Vehicle;

struct Food {
    Vec2D position;
    // prevent the initial food from all disappearing at once
    int  lifespan = random_in_range(850, 1750);  // lifespan in simulation ticks
    bool consumed = false;
    double nutrition;

    Food();

    Food(double nutrition);

    Food(Vec2D const& pos);

    Food(Vec2D const& pos, double nutrition);

    [[nodiscard]] double get_nutrition() const;

    [[nodiscard]] Vec2D const& get_position() const;

    void update();

    void consume(Vehicle& consumer);
};

#endif  // FOOD_H