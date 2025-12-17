#include "vec2d.h"
#include <cmath>
#include <iostream>
#include "utils.h"

namespace tom {

Vec2D::Vec2D() : x(0), y(0)
{
}

Vec2D::Vec2D(double x, double y) : x(x), y(y)
{
}
[[nodiscard]] Vec2D Vec2D::operator+(Vec2D const& other) const
{
    return {x + other.x, y + other.y};
}
[[nodiscard]] Vec2D Vec2D::operator-(Vec2D const& other) const
{
    return {x - other.x, y - other.y};
}
[[nodiscard]] Vec2D Vec2D::operator*(double scalar) const
{
    return {x * scalar, y * scalar};
}
[[nodiscard]] Vec2D Vec2D::operator/(double scalar) const
{
    return {x / scalar, y / scalar};
}
Vec2D& Vec2D::operator+=(Vec2D const& other)
{
    x += other.x;
    y += other.y;
    return *this;
}

Vec2D& Vec2D::operator-=(Vec2D const& other)
{
    x -= other.x;
    y -= other.y;
    return *this;
}
Vec2D& Vec2D::operator*=(double scalar)
{
    x *= scalar;
    y *= scalar;
    return *this;
}

Vec2D& Vec2D::operator/=(double scalar)
{
    x /= scalar;
    y /= scalar;
    return *this;
}
[[nodiscard]] double Vec2D::distance_to(Vec2D const& other) const
{
    double dx = x - other.x;
    double dy = y - other.y;
    return std::sqrt(dx * dx + dy * dy);
}

[[nodiscard]] double Vec2D::magnitude() const
{
    return std::sqrt(x * x + y * y);
}

void Vec2D::set_mag(double mag)
{
    normalize();
    x *= mag;
    y *= mag;
}

void Vec2D::normalize()
{
    double mag = magnitude();
    if (mag != 0) {
        set(x / mag, y / mag);
    }
}

[[nodiscard]] Vec2D Vec2D::normalized() const
{
    double mag = magnitude();
    if (mag == 0)
        return {0, 0};
    return {x / mag, y / mag};
}

void Vec2D::limit(double max)
{
    double mag = magnitude();
    if (mag > max) {
        set((x / mag) * max, (y / mag) * max);
    }
}

void Vec2D::set_heading(double angle)
{
    double mag = magnitude();
    set(cos(angle) * mag, sin(angle) * mag);
}

[[nodiscard]] double Vec2D::heading() const
{
    return atan2(y, x);
}

[[nodiscard]] Vec2D Vec2D::copy() const
{
    return {x, y};
}

void Vec2D::rotate(double angle)
{
    double cos_a = cos(angle);
    double sin_a = sin(angle);
    double new_x = x * cos_a - y * sin_a;
    double new_y = x * sin_a + y * cos_a;
    set(new_x, new_y);
}

[[nodiscard]] Vec2D Vec2D::rotated(double angle) const
{
    double cos_a = cos(angle);
    double sin_a = sin(angle);
    return {x * cos_a - y * sin_a, x * sin_a + y * cos_a};
}

void Vec2D::reset()
{
    x = 0;
    y = 0;
}

void Vec2D::set(double new_x, double new_y)
{
    x = new_x;
    y = new_y;
}

[[nodiscard]] double Vec2D::dot(Vec2D const& other) const
{
    return x * other.x + y * other.y;
}

[[nodiscard]] double Vec2D::angle_between(Vec2D const& other) const
{
    return acos(dot(other) / (magnitude() * other.magnitude()));
}

Vec2D Vec2D::random(double magnitude)
{
    double angle = random_in_range(0, 2 * M_PI);
    return {cos(angle) * magnitude, sin(angle) * magnitude};
}

std::ostream& operator<<(std::ostream& os, Vec2D const& vec)
{
    os << "(" << vec.x << ", " << vec.y << ")";
    return os;
}

}  // namespace tom