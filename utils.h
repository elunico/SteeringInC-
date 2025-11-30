#ifndef UTILS_H
#define UTILS_H

#include <cstdlib>
#include <string>
#include <cassert>

#ifndef NDEBUG
#define UNREACHABLE() assert(false && "Unreachable code reached.")
#else
#define UNREACHABLE() std::unreachable();
#endif


double randomDelta(double scale = 0.1);

double randomInRange(double min, double max);

void log(std::string const& message);

constexpr bool doubleEqual(double a, double b, double epsilon = 0.001);

constexpr bool doubleNearZero(double a, double epsilon = 0.001);

template <typename T>
T remap(T value, T from1, T to1, T from2, T to2)
{
    return (value - from1) / (to1 - from1) * (to2 - from2) + from2;
}

#endif  // UTILS_H