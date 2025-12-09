#ifndef UTILS_H
#define UTILS_H

#include <cassert>
#include <concepts>
#include <cstdlib>
#include <iostream>
#include <string>
#include "vec2d.h"

#ifndef NDEBUG
#define UNREACHABLE() assert(false && "Unreachable code reached.")
#else
#define UNREACHABLE() std::unreachable();
#endif

namespace tom {

template <typename T>
concept Positionable = requires(T a) {
    { a.get_position() } -> std::convertible_to<Vec2D>;
};

double random_delta(double scale = 0.1) noexcept;

double random_in_range(double min, double max) noexcept;

void log(std::string const& message);

constexpr bool double_equal(double a, double b, double epsilon = 0.001) noexcept
{
    return std::abs(a - b) <= epsilon;
}

constexpr bool double_near_zero(double a, double epsilon = 0.001) noexcept
{
    return double_equal(a, 0.0, epsilon);
}

template <typename T>
concept Numeric = !std::is_pointer_v<T> && requires(T a, T b) {
    a - b;
    a / b;
    a * b;
    a + b;
};

template <typename T>
    requires Numeric<T>
T remap(T value, T from1, T to1, T from2, T to2)
{
    return (value - from1) / (to1 - from1) * (to2 - from2) + from2;
}

template <typename... Args>
void output(Args&&... args)
{
    ((std::cout << std::forward<Args>(args)), ...);
}

}  // namespace tom

#endif  // UTILS_H