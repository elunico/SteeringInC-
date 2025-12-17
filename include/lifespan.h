#ifndef LIFESPAN_H
#define LIFESPAN_H

#include <type_traits>
#include "utils.h"

namespace tom {

template <typename T, T tick_amt>
    requires std::is_arithmetic_v<T>
class Lifespan {
    using UnderlyingType = T;

    constexpr static T tick_amount = tick_amt;

    T    life;
    bool unlimited_ = false;

   public:
    Lifespan(T ticks) noexcept : life(ticks)
    {
    }

    static Lifespan unlimited() noexcept
    {
        Lifespan ls(0);
        ls.unlimited_ = true;
        return ls;
    }

    static Lifespan random(T min_ticks, T max_ticks) noexcept
    {
        return {T(random_in_range(min_ticks, max_ticks))};
    }

    Lifespan& operator--() noexcept
    {
        update();
        return *this;
    }

    Lifespan& operator--(int) noexcept
    {
        update();
        return *this;
    }

    Lifespan& operator+=(T ticks) noexcept
    {
        if (!unlimited_) {
            life += ticks;
        }
        return *this;
    }

    Lifespan& operator-=(T ticks) noexcept
    {
        if (!unlimited_) {
            life -= ticks;
            if (life < 0) {
                life = 0;
            }
        }
        return *this;
    }

    void update() noexcept
    {
        if (!unlimited_ && life > 0) {
            life -= tick_amount;
        }
    }

    [[nodiscard]] T remaining() const noexcept
    {
        return life;
    }

    virtual void expire() noexcept
    {
        expire(false);
    }

    virtual void expire(bool force) noexcept
    {
        if (force || !unlimited_)
            life = 0;
    }

    [[nodiscard]] bool is_expired() const noexcept
    {
        return !unlimited_ && life <= 0;
    }

    bool operator==(Lifespan const& other) const noexcept
    {
        return life == other.life && unlimited_ == other.unlimited_;
    }

    bool operator!=(Lifespan const& other) const noexcept
    {
        return !(*this == other);
    }

    bool operator<(Lifespan const& other) const noexcept
    {
        return life < other.life && unlimited_ == other.unlimited_;
    }

    bool operator<=(Lifespan const& other) const noexcept
    {
        return life <= other.life && unlimited_ == other.unlimited_;
    }

    bool operator>(Lifespan const& other) const noexcept
    {
        return life > other.life && unlimited_ == other.unlimited_;
    }

    bool operator>=(Lifespan const& other) const noexcept
    {
        return life >= other.life && unlimited_ == other.unlimited_;
    }

    bool operator<(UnderlyingType const& value) const noexcept
    {
        return life < value && !unlimited_;
    }

    bool operator<=(UnderlyingType const& value) const noexcept
    {
        return life <= value && !unlimited_;
    }

    bool operator>(UnderlyingType const& value) const noexcept
    {
        return life > value && !unlimited_;
    }

    bool operator>=(UnderlyingType const& value) const noexcept
    {
        return life >= value && !unlimited_;
    }

    virtual ~Lifespan() = default;
};

using IntLifespan    = Lifespan<int, 1>;
using DoubleLifespan = Lifespan<double, 0.05>;

}  // namespace tom

#endif  // LIFESPAN_H