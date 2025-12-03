#include "utils.h"
#include <fstream>

double random_delta(double scale)
{
    return ((rand() % 200) / 100.0 - 1.0) * scale;  // small random change
}

double random_in_range(double min, double max)
{
    return min + (rand() / (RAND_MAX / (max - min)));
}

void log(std::string const& message)
{
    static std::ofstream log_file("simulation_log.txt", std::ios_base::app);
    log_file << message << std::endl;
}
