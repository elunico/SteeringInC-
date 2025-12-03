#include "food.h"
#include <cassert>

#include "vec2d.h"
#include "vehicle.h"

Food::IdType Food::global_id_counter = 1;

[[nodiscard]] double Food::get_nutrition() const
{
    return nutrition;
}

void Food::update()
{
    if (lifespan > 0) {
        lifespan--;
    } else {
        consumed = true;
    }
}

void Food::consume(Vehicle& consumer)
{
    consumer.health += nutrition;
    consumed = true;
}

Vec2D const& Food::get_position() const
{
    return position;
}

Food::Food()
    : id(global_id_counter), position(Vec2D::random(100)), nutrition(5.0)
{
}

Food::Food(Vec2D const& pos)
    : id(global_id_counter), position(pos), nutrition(5.0)
{
}

Food::Food(Vec2D const& pos, double nutrition)
    : id(global_id_counter), position(pos), nutrition(nutrition)
{
}
