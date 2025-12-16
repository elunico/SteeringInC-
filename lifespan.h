#ifndef LIFESPAN_H
#define LIFESPAN_H

#include "utils.h"

namespace tom {

class Lifespan {
    int  remaining_ticks;
    bool unlimited_ = false;

   public:
    Lifespan(int ticks) noexcept;

    static Lifespan unlimited() noexcept
    {
        Lifespan ls(0);
        ls.unlimited_ = true;
        return ls;
    }

    static Lifespan random(int min_ticks, int max_ticks) noexcept
    {
        return {static_cast<int>(random_in_range(min_ticks, max_ticks))};
    }

    Lifespan& operator--() noexcept;

    Lifespan& operator--(int) noexcept;

    void update() noexcept;

    [[nodiscard]] int remaining() const noexcept;

    virtual void expire() noexcept;

    virtual void expire(bool force) noexcept;

    [[nodiscard]] bool is_expired() const noexcept;

    virtual ~Lifespan();
};

}  // namespace tom

#endif  // LIFESPAN_H