#include "vec2d.h"
#include <cmath>
#include <iostream>

Vec2D::Vec2D() : x(0), y(0)
{
}
Vec2D::Vec2D(double x, double y) : x(x), y(y)
{
}
[[nodiscard]] Vec2D Vec2D::operator+(Vec2D const& other) const
{
    return Vec2D(x + other.x, y + other.y);
}
[[nodiscard]] Vec2D Vec2D::operator-(Vec2D const& other) const
{
    return Vec2D(x - other.x, y - other.y);
}
[[nodiscard]] Vec2D Vec2D::operator*(double scalar) const
{
    return Vec2D(x * scalar, y * scalar);
}
[[nodiscard]] Vec2D Vec2D::operator/(double scalar) const
{
    return Vec2D(x / scalar, y / scalar);
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

[[nodiscard]] double Vec2D::distanceTo(Vec2D const& other) const
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
        x /= mag;
        y /= mag;
    }
}

[[nodiscard]] Vec2D Vec2D::normalized() const
{
    double mag = magnitude();
    if (mag == 0)
        return Vec2D(0, 0);
    return Vec2D(x / mag, y / mag);
}

void Vec2D::limit(double max)
{
    double mag = magnitude();
    if (mag > max) {
        x = (x / mag) * max;
        y = (y / mag) * max;
    }
}

void Vec2D::set_heading(double angle)
{
    double mag = magnitude();
    x          = cos(angle) * mag;
    y          = sin(angle) * mag;
}

[[nodiscard]] double Vec2D::heading() const
{
    return atan2(y, x);
}

[[nodiscard]] Vec2D Vec2D::copy() const
{
    return Vec2D(x, y);
}

void Vec2D::rotate(double angle)
{
    double cosA = cos(angle);
    double sinA = sin(angle);
    double newX = x * cosA - y * sinA;
    double newY = x * sinA + y * cosA;
    x           = newX;
    y           = newY;
}

[[nodiscard]] Vec2D Vec2D::rotated(double angle) const
{
    double cosA = cos(angle);
    double sinA = sin(angle);
    return Vec2D(x * cosA - y * sinA, x * sinA + y * cosA);
}

void Vec2D::reset()
{
    x = 0;
    y = 0;
}

void Vec2D::set(double newX, double newY)
{
    x = newX;
    y = newY;
}

std::ostream& operator<<(std::ostream& os, Vec2D const& vec)
{
    os << "(" << vec.x << ", " << vec.y << ")";
    return os;
}
