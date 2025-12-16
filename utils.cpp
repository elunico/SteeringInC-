#include "utils.h"
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

[[nodiscard]] std::vector<std::string> split(std::string const& str,
                                             char               delimiter)
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

std::vector<std::string> split(std::string const& str,
                               char               delimiter,
                               size_t             limit)
{
    std::vector<std::string> parts;
    std::string              current;
    if (limit == 0) {
        return {};
    }
    if (limit == 1) {
        return {str};
    }
    std::size_t char_count = 0;
    for (char c : str) {
        if (c == delimiter) {
            parts.push_back(current);
            current.clear();
        } else {
            current += c;
        }
        char_count++;
        if (parts.size() >= static_cast<size_t>(limit - 1)) {
            parts.emplace_back(str.begin() + char_count, str.end());
            return parts;
        }
    }
    parts.push_back(current);
    return parts;
}

}  // namespace tom