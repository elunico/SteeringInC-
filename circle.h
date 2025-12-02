//
// Created by Thomas Povinelli on 11/29/25.
//

#ifndef CIRCLE_H
#define CIRCLE_H
#include "vec2d.h"

class Circle {
    Vec2D  center;
    double radius;

    Circle(Vec2D const& center, double radius);

    bool contains(Vec2D const& point) const;

    bool intersects(Circle const& other) const;
};

#endif  // CIRCLE_H
