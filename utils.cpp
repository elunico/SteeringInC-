#include "utils.h"

double randomDelta(double scale)
{
    return ((rand() % 200) / 100.0 - 1.0) * scale;  // small random change
}

double randomInRange(double min, double max)
{
    return min + (rand() / (RAND_MAX / (max - min)));
}