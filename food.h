#ifndef FOOD_H
#define FOOD_H

#include "utils.h"
#include "vec2d.h"
#include "world.h"

class Vehicle;

struct Food {
    using IdType = World::Food_id_type;
    IdType id;
    Vec2D  position;
    // prevent the initial food from all disappearing at once
    int  lifespan = random_in_range(850, 1750);  // lifespan in simulation ticks
    bool consumed = false;
    double nutrition;

    Food();

    Food(Vec2D const& pos);

    Food(Vec2D const& pos, double nutrition);

    [[nodiscard]] double get_nutrition() const;

    [[nodiscard]] Vec2D const& get_position() const;

    void update();

    void consume(Vehicle& consumer);

    static  IdType next_id() { return ++global_id_counter; }

   private:
    static IdType global_id_counter;
};

#endif  // FOOD_H