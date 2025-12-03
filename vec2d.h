#ifndef VEC2D_H
#define VEC2D_H
#include <iostream>

struct Vec2D {
    double x;
    double y;

    Vec2D();
    Vec2D(double x, double y);
    Vec2D(Vec2D const& other) = default;
    [[nodiscard]] Vec2D operator+(Vec2D const& other) const;
    [[nodiscard]] Vec2D operator-(Vec2D const& other) const;
    [[nodiscard]] Vec2D operator*(double scalar) const;
    [[nodiscard]] Vec2D operator/(double scalar) const;

    Vec2D& operator=(Vec2D const& other) = default;

    Vec2D& operator+=(Vec2D const& other);
    Vec2D& operator-=(Vec2D const& other);
    Vec2D& operator*=(double scalar);
    Vec2D& operator/=(double scalar);

    [[nodiscard]] double distance_to(Vec2D const& other) const;
    [[nodiscard]] double magnitude() const;
    void                 set_mag(double mag);
    void                 normalize();
    [[nodiscard]] Vec2D  normalized() const;
    void                 limit(double max);
    void                 set_heading(double angle);
    [[nodiscard]] double heading() const;
    [[nodiscard]] Vec2D  copy() const;
    void                 rotate(double angle);
    [[nodiscard]] Vec2D  rotated(double angle) const;
    void                 reset();
    void                 set(double new_x, double new_y);

    static Vec2D random(double magnitude = 1.0);
};

std::ostream& operator<<(std::ostream& os, Vec2D const& v);

#endif  // VEC2D_H