#include "split.h"

namespace tom {

SplitLimit::SplitLimit(size_t l) noexcept : limit(l)
{
}

SplitLimit::operator size_t() const noexcept
{
    return limit;
}

bool SplitLimit::operator==(size_t other) const noexcept
{
    return limit == other;
}

bool SplitLimit::operator!=(size_t other) const noexcept
{
    return limit != other;
}

[[nodiscard]] bool SplitLimit::is_unlimited() const noexcept
{
    return limit == static_cast<size_t>(-1);
}

[[nodiscard]] std::vector<std::string> split(std::string const& str,
                                             char               delimiter,
                                             SplitLimit         limit)
{
    std::vector<std::string> parts;
    std::string              current;
    if (limit == 0ul) {
        return {};
    }
    if (limit == 1ul) {
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
        if (!limit.is_unlimited() &&
            parts.size() >= static_cast<size_t>(limit) - 1) {
            parts.emplace_back(str.begin() + char_count, str.end());
            return parts;
        }
    }
    parts.push_back(current);
    return parts;
}

[[nodiscard]] std::vector<std::string> split(std::string const& str,
                                             char               delimiter)
{
    return split(str, delimiter, SplitLimit::unlimited());
}

}  // namespace tom