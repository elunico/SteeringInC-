#ifndef OPTIONSET_H
#define OPTIONSET_H

#include <iostream>
#include <unordered_set>

namespace tom {

template <typename T>
struct OptionSet {
    std::unordered_set<T> options;
    OptionSet()
    {
    }

    explicit OptionSet(T option)
    {
        options.insert(option);
    }

    bool operator==(T option) const
    {
        return contains(option);
    }

    bool operator!=(T option) const
    {
        return !(operator==(option));
    }

    bool operator==(OptionSet<T> const& other) const
    {
        return options == other.options;
    }

    bool operator!=(OptionSet<T> const& other) const
    {
        return !(options == other.options);
    }

    void set(T option)
    {
        options.clear();
        options.insert(option);
    }

    void toggle(T option)
    {
        if (contains(option))
            remove(option);
        else
            add(option);
    }

    void add(T option)
    {
        options.insert(option);
    }

    void remove(T option)
    {
        auto i = options.find(option);
        if (i != options.end()) {
            options.erase(i);
        }
    }

    void clear()
    {
        options.clear();
    }

    [[nodiscard]]
    bool contains(T option) const
    {
        return options.contains(option);
    }
};

template <typename T>
static inline std::ostream& operator<<(std::ostream&       os,
                                       OptionSet<T> const& state)
{
    os << "OptionSet(";
    auto i = state.options.cbegin();
    while (i != state.options.cend()) {
        os << *i++;
        if (i == state.options.cend())
            break;
        os << " | ";
    }
    os << ")";
    return os;
}

}  // namespace tom

#endif
