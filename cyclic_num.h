#ifndef CYCLIC_NUM_H
#define CYCLIC_NUM_H

#include <type_traits>

namespace tom {

template <typename T, T maximum>
    requires std::is_arithmetic_v<T>
/**
 *  Class representing modular arithmetic using the underlying value of type T
 *
 *  The class will force the value to be in the interval [0, maximum)
 */
class cyclic {
    using NumberType = T;
    using max        = std::integral_constant<T, maximum>;
    T value;

   public:
    constexpr cyclic() : value(0)
    {
    }

    /**
     * @brief Constructs a cyclic object with the initial value.
     *
     * This constructor initializes the cyclic object with the provided initial
     * value. The value is adjusted to ensure it stays within the specified
     * modulus range [0, maximum), using the modulo operation.
     *
     * @tparam T The arithmetic type of the cyclic value.
     * @param initial The initial value to assign to the cyclic object.
     *                It will be reduced modulo the maximum value.
     */
    constexpr cyclic(T initial) : value(initial % maximum)
    {
    }

    /**
     * Set a new value which wraps around in the range [0, maximum)
     *
     * @param new_value the new value which will be wrapped into the interval
     * [0, maximum)
     */
    constexpr void set(T new_value)
    {
        value = new_value % maximum;
    }

    constexpr T get() const
    {
        return value;
    }

    constexpr auto& operator++()
    {
        value = (value + 1) % maximum;
        return *this;
    }

    constexpr auto operator++(int)
    {
        auto temp = *this;
        value     = (value + 1) % maximum;
        return temp;
    }

    constexpr auto& operator--()
    {
        value = (value - 1 + maximum) % maximum;
        return *this;
    }

    constexpr auto operator--(int)
    {
        auto temp = *this;
        value     = (value - 1 + maximum) % maximum;
        return temp;
    }

    constexpr cyclic operator+(T rhs) const
    {
        return value + rhs;
    }

    constexpr cyclic operator-(T rhs) const
    {
        return value - rhs;
    }

    constexpr auto& operator+=(T rhs)
    {
        value = (value + rhs) % maximum;
        return *this;
    }

    constexpr auto& operator-=(T rhs)
    {
        value = (value - rhs + maximum) % maximum;
        return *this;
    }

    constexpr operator T() const
    {
        return value;
    }

    constexpr T operator*() const
    {
        return value;
    }
};

}  // namespace tom

#endif  // CYCLIC_NUM_H