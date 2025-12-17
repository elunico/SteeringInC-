#ifndef UTILS_H
#define UTILS_H

#include <cassert>
#include <concepts>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <string>
#include <type_traits>
#include <vector>
#include "vec2d.h"

#ifndef NDEBUG
#define UNREACHABLE() assert(false && "Unreachable code reached.")
#else
#define UNREACHABLE() std::unreachable();
#endif

namespace tom {

template <typename T, T tick_amt>
    requires std::is_arithmetic_v<T>
class Lifespan;

template <typename T>
concept Positionable = requires(T a) {
    { a.get_position() } -> std::convertible_to<Vec2D>;
};

static std::random_device rd;
static std::mt19937       gen(rd());

template <typename T>
    requires requires(T t) { gen.seed(t); }
void set_seed(T seed)
{
    gen.seed(seed);
}

bool random_bool() noexcept;

double random_delta(double scale = 0.1) noexcept;

double random_in_range(double min, double max) noexcept;

int random_int(int min, int max) noexcept;

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

#ifdef NDEBUG
#define debug_output(...) ((void) 0)
#define DEBUG_USE(...) ((void) 0)
#else
#define debug_output(...) ::tom::output(__VA_ARGS__)
#define DEBUG_USE(...) __VA_ARGS__
#endif

template <int n>
auto get(tom::Vec2D const& v) -> decltype(auto)
{
    if constexpr (n == 0)
        return v.x;
    else
        return v.y;
}

template <typename T, typename R = T>
R constrain(T value, T min, T max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

template <typename T>
// fallback to std::midpoint when available
T midpoint(T a, T b)
{
    return std::midpoint(a, b);
}

template <>
inline Vec2D midpoint(Vec2D a, Vec2D b)
{
    return Vec2D{std::midpoint(a.x, b.x), std::midpoint(a.y, b.y)};
}

template <typename T, typename R>
    requires std::same_as<T, std::string>
R midpoint(T s)
{
    return (s.length() + 1) / 2;
}

template <typename T, typename R = size_t>
    requires std::same_as<
        std::add_pointer_t<std::remove_cv_t<std::remove_pointer_t<T> > >,
        char*>
R midpoint(T s)
{
    return (strlen(s) + 1) / 2;
}

template <typename T, T amt>
Lifespan<T, amt> midpoint(Lifespan<T, amt> const& a, Lifespan<T, amt> const& b)
{
    return Lifespan<T, amt>(std::midpoint(a.remaining(), b.remaining()));
}
}  // namespace tom

#include <tuple>

// 1. tuple_size
// Tells the compiler how many elements are in the "tuple"
template <>
struct std::tuple_size<tom::Vec2D> : std::integral_constant<size_t, 2> {};

// 2. tuple_element
// Tells the compiler what the type of the Nth element is.
// We use decltype(declval...) so this updates automatically if you change float
// to double.

// Element 0 -> Type of x
template <>
struct std::tuple_element<0, tom::Vec2D> {
    using type = decltype(std::declval<tom::Vec2D>().x);
};

// Element 1 -> Type of y
template <>
struct std::tuple_element<1, tom::Vec2D> {
    using type = decltype(std::declval<tom::Vec2D>().y);
};

#endif  // UTILS_H