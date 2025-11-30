#include "rectangle.h"

Rectangle::Rectangle() : topLeft(Vec2D(0, 0)), bottomRight(Vec2D(0, 0))
{
}

Rectangle::Rectangle(Vec2D topLeft, Vec2D bottomRight)
    : topLeft(topLeft), bottomRight(bottomRight)
{
}

bool Rectangle::contains(Vec2D const& point) const
{
    return (point.x >= topLeft.x && point.x <= bottomRight.x &&
            point.y >= topLeft.y && point.y <= bottomRight.y);
}

bool Rectangle::intersects(Rectangle const& other) const
{
    return !(
        other.topLeft.x >= bottomRight.x || other.bottomRight.x <= topLeft.x ||
        other.topLeft.y >= bottomRight.y || other.bottomRight.y <= topLeft.y);
}
