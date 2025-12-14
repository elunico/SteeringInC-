
#ifndef ENUMERATION_H
#define ENUMERATION_H

#include <concepts>

namespace tom {

template <std::integral T>
struct Enumeration {
    T value;

    constexpr virtual ~Enumeration() = default;

    constexpr operator T() const
    {
        return value;
    }

    constexpr Enumeration(T v) : value(v)
    {
    }

    constexpr Enumeration operator|(Enumeration other) const
    {
        return Enumeration(value | other.value);
    }

    constexpr Enumeration operator&(Enumeration other) const
    {
        return Enumeration(value & other.value);
    }

    constexpr void toggle(Enumeration mode)
    {
        if (is_set(mode)) {
            unset(mode);
        } else {
            set(mode);
        }
    }

    constexpr bool is_set(Enumeration mode) const
    {
        return (value & mode.value) != 0;
    }

    constexpr void clear()
    {
        value = 0;
    }

    constexpr void set_only(Enumeration mode)
    {
        value = 0;
        set(mode);
    }

    constexpr bool is_disjoint(Enumeration mode) const
    {
        return (value & mode.value) == 0;
    }

    constexpr void set(Enumeration mode)
    {
        value |= mode.value;
    }

    constexpr void unset(Enumeration mode)
    {
        value &= ~mode.value;
    }
};

}  // namespace tom

#endif