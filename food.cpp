#include "food.h"
#include <cassert>

#include "vec2d.h"
#include "vehicle.h"

Food::IdType Food::global_id_counter = 1;

[[nodiscard]] double Food::get_nutrition() const noexcept
{
    return nutrition;
}

void Food::update() noexcept
{
    if (lifespan > 0) {
        lifespan--;
    } else {
        consumed = true;
    }
}

void Food::consume(Vehicle& consumer) noexcept
{
    consumer.health += nutrition;
    consumed = true;
}

Vec2D const& Food::get_position() const noexcept
{
    return position;
}

Food::Food() noexcept
    : id(global_id_counter), position(Vec2D::random(100)), nutrition(5.0)
{
}

Food::Food(Vec2D const& pos) noexcept
    : id(global_id_counter), position(pos), nutrition(5.0)
{
}

Food::Food(Vec2D const& pos, double nutrition) noexcept
    : id(global_id_counter), position(pos), nutrition(nutrition)
{
}
