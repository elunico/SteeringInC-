#ifndef FOOD_H
#define FOOD_H

#include "utils.h"
#include "vec2d.h"
#include "world.h"

class Vehicle;

struct Food {
    using IdType = World::FoodIdType;
    IdType id;
    Vec2D  position;
    // prevent the initial food from all disappearing at once
    int  lifespan = random_in_range(850, 1750);  // lifespan in simulation ticks
    bool consumed = false;
    double nutrition;

    Food() noexcept;

    Food(Vec2D const& pos) noexcept;

    Food(Vec2D const& pos, double nutrition) noexcept;
    [[nodiscard]] double get_nutrition() const noexcept;

    [[nodiscard]] Vec2D const& get_position() const noexcept;

    void update() noexcept;

    void consume(Vehicle& consumer) noexcept;

    static IdType next_id() noexcept
    {
        return ++global_id_counter;
    }

   private:
    static IdType global_id_counter;
};

#endif  // FOOD_H