#include "utils.h"
#include <fstream>

#include <random>

namespace tom {

static std::random_device rd;
static std::mt19937       gen(rd());

double random_delta(double scale) noexcept
{
    return ((rand() % 200) / 100.0 - 1.0) * scale;  // small random change
}

double random_in_range(double min, double max) noexcept
{
    return min + (rand() / (RAND_MAX / (max - min)));
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

std::vector<std::string> split(std::string const& str, char delimiter)
{
    std::vector<std::string> parts;
    std::string              current;
    for (char c : str) {
        if (c == delimiter) {
            parts.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }
    parts.push_back(current);
    return parts;
}

}  // namespace tom