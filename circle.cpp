//
// Created by Thomas Povinelli on 11/29/25.
//

#include "circle.h"

Circle::Circle(Vec2D const& center, double radius): center(center), radius(radius) {}

bool Circle::contains(Vec2D const& point) const
{
    return center.distanceTo(point) <= radius;
}

bool Circle::intersects(Circle const& other) const
{
    if (center.distanceTo(other.center) > radius + other.radius) {
        return false;
    }
    return center.distanceTo(other.center) < radius + other.radius;
}