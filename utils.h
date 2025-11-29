#ifndef UTILS_H
#define UTILS_H

#include <cstdlib>
#include <string>

double randomDelta(double scale = 0.1);

double randomInRange(double min, double max);

void log(std::string const& message);

#endif  // UTILS_H