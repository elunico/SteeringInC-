#ifndef CYCLIC_NUM_H
#define CYCLIC_NUM_H

#include <type_traits>

namespace tom {

template <typename T, T maximum>
    requires std::is_arithmetic<T>::value
class cyclic {
    using NumberType = T;
    using max        = std::integral_constant<T, maximum>;
    // T maximum;
    T value;

   public:
    cyclic() : value(0)
    {
    }

    cyclic(T initial) : value(initial % maximum)
    {
    }
    void set(T new_value)
    {
        value = new_value % maximum;
    }

    T get() const
    {
        return value;
    }

    auto& operator++()
    {
        value = (value + 1) % maximum;
        return *this;
    }

    auto operator++(int)
    {
        auto temp = *this;
        value     = (value + 1) % maximum;
        return temp;
    }

    auto& operator--()
    {
        value = (value - 1 + maximum) % maximum;
        return *this;
    }

    auto operator--(int)
    {
        auto temp = *this;
        value     = (value - 1 + maximum) % maximum;
        return temp;
    }

    auto& operator+=(T rhs)
    {
        value = (value + rhs) % maximum;
        return *this;
    }

    auto& operator-=(T rhs)
    {
        value = (value - rhs + maximum) % maximum;
        return *this;
    }

    operator T() const
    {
        return value;
    }

    T operator*() const
    {
        return value;
    }
};

}  // namespace tom
#endif  // CYCLIC_NUM_H