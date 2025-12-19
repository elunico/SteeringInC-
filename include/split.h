#ifndef SPLIT_H
#define SPLIT_H

#include <cstdlib>
#include <limits>
#include <string>
#include <vector>

namespace tom {

class SplitLimit {
    size_t limit;

   public:
    SplitLimit(size_t l) noexcept;

    operator size_t() const noexcept;

    bool operator==(size_t other) const noexcept;

    bool operator!=(size_t other) const noexcept;

    bool is_unlimited() const noexcept;

    static SplitLimit unlimited() noexcept
    {
        static_assert(
            static_cast<size_t>(-1) >=
            std::numeric_limits<std::vector<std::string>::size_type>::max());
        return SplitLimit{static_cast<size_t>(-1)};
    }
};

[[nodiscard]] std::vector<std::string> split(std::string const& str,
                                             char               delimiter,
                                             SplitLimit         limit);

[[nodiscard]] std::vector<std::string> split(std::string const& str,
                                             char               delimiter);

};  // namespace tom

#endif  // SPLIT_H