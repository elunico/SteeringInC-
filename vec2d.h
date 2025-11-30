#ifndef VEC2D_H
#define VEC2D_H
#include <iosfwd>

struct Vec2D {
    double x;
    double y;

    Vec2D();
    Vec2D(double x, double y);
    [[nodiscard]] Vec2D operator+(Vec2D const& other) const;
    [[nodiscard]] Vec2D operator-(Vec2D const& other) const;
    [[nodiscard]] Vec2D operator*(double scalar) const;
    [[nodiscard]] Vec2D operator/(double scalar) const;

    Vec2D& operator+=(Vec2D const& other);
    Vec2D& operator-=(Vec2D const& other);
    Vec2D& operator*=(double scalar);
    Vec2D& operator/=(double scalar);

    [[nodiscard]] double distanceTo(Vec2D const& other) const;
    [[nodiscard]] double magnitude() const;
    void                 setMag(double mag);
    void                 normalize();
    [[nodiscard]] Vec2D  normalized() const;
    void                 limit(double max);
    void                 setHeading(double angle);
    [[nodiscard]] double heading() const;
    [[nodiscard]] Vec2D  copy() const;
    void                 rotate(double angle);
    [[nodiscard]] Vec2D  rotated(double angle) const;
    void                 reset();
    void                 set(double newX, double newY);
};

std::ostream& operator<<(std::ostream& os, Vec2D const& v);

#endif  // VEC2D_H