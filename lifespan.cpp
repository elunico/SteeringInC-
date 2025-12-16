#include "lifespan.h"

namespace tom {

Lifespan::Lifespan(int ticks) noexcept : remaining_ticks(ticks)
{
}

Lifespan& Lifespan::operator--() noexcept
{
    update();
    return *this;
}

Lifespan& Lifespan::operator--(int) noexcept
{
    update();
    return *this;
}

void Lifespan::update() noexcept
{
    if (!unlimited_ && remaining_ticks > 0) {
        --remaining_ticks;
    }
}
int Lifespan::remaining() const noexcept
{
    return remaining_ticks;
}

void Lifespan::expire() noexcept
{
    expire(false);
}

void Lifespan::expire(bool force) noexcept
{
    if (force || !unlimited_)
        remaining_ticks = 0;
}

[[nodiscard]] bool Lifespan::is_expired() const noexcept
{
    return !unlimited_ && remaining_ticks <= 0;
}

Lifespan::~Lifespan() = default;

}  // namespace tom