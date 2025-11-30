#include "utils.h"
#include <fstream>

double randomDelta(double scale)
{
    return ((rand() % 200) / 100.0 - 1.0) * scale;  // small random change
}

double randomInRange(double min, double max)
{
    return min + (rand() / (RAND_MAX / (max - min)));
}

void log(std::string const& message)
{
    static std::ofstream logFile("simulation_log.txt", std::ios_base::app);
    logFile << message << std::endl;
}

constexpr bool doubleEqual(double a, double b, double epsilon)
{
    return std::abs(a - b) <= epsilon;
}

constexpr bool doubleNearZero(double a, double epsilon)
{
    return doubleEqual(a, 0.0, epsilon);
}