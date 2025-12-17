#include "utils.h"
#include <cassert>
#include <cstddef>
#include <fstream>

#include <random>

namespace tom {

bool random_bool() noexcept
{
    std::uniform_int_distribution<> dis(0, 1);
    return dis(gen) == 1;
}

double random_delta(double scale) noexcept
{
    // return ((rand() % 200) / 100.0 - 1.0) * scale;  // small random change
    std::uniform_real_distribution<> dis(-scale, scale);
    return dis(gen);
}

double random_in_range(double min, double max) noexcept
{
    // return min + (rand() / (RAND_MAX / (max - min)));
    std::uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

int random_int(int min, int max) noexcept
{
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

void log(std::string const& message)
{
    static std::ofstream log_file("simulation_log.txt", std::ios_base::app);
    log_file << message << std::endl;
}

}  // namespace tom