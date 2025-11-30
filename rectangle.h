#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "vec2d.h"

struct Rectangle {
    Vec2D topLeft;
    Vec2D bottomRight;

    Rectangle();
    Rectangle(Vec2D topLeft, Vec2D bottomRight);

    [[nodiscard]] bool contains(Vec2D const& point) const;

    [[nodiscard]] bool intersects(Rectangle const& other) const;
};

#endif  // RECTANGLE_H