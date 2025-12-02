#include "food.h"

#include "vehicle.h"

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

Food::Food() : position(Vec2D{0.0, 0.0}), nutrition(5.0)
{
}

Food::Food(double nutrition) : position(Vec2D{0.0, 0.0}), nutrition(nutrition)
{
}

Food::Food(Vec2D const& pos) : position(pos), nutrition(5.0)
{
}

Food::Food(Vec2D const& pos, double nutrition)
    : position(pos), nutrition(nutrition)
{
}