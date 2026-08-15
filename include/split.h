#ifndef SPLIT_H
#define SPLIT_H

#include <cstdlib>
#include <optional>
#include <string>
#include <vector>

namespace tom {

template <typename Container, typename Element = Container::value_type>
[[nodiscard]] std::vector<Container> split(Container const&           container,
                                           Element                    delimiter,
                                           std::optional<std::size_t> limit)
{
    if (limit == 0) {
        return {container};
    }
    std::size_t            char_count = 0;
    std::vector<Container> parts;
    Container              current;
    for (auto c : container) {
        if (c == delimiter) {
            parts.push_back(current);
            current.clear();
        } else {
            current.push_back(c);
        }
        char_count++;
        if (limit.has_value() && parts.size() >= static_cast<size_t>(*limit)) {
            parts.emplace_back(container.begin() + char_count, container.end());
            return parts;
        }
    }
    parts.push_back(current);
    return parts;
}

// template <>
// [[nodiscard]] std::vector<std::string> split<char const* const&, char>(
//     char const* const&         s,
//     char                       delimiter,
//     std::optional<std::size_t> limit)
// {
//     return split(std::string(s), delimiter, limit);
// }

template <typename Container, typename Element = Container::value_type>
[[nodiscard]] std::vector<Container> split(Container const& container,
                                           Element          delimiter)
{
    return split(container, delimiter, std::nullopt);
}

};  // namespace tom

#endif  // SPLIT_H