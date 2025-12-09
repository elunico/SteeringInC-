#include "food.h"
#include <cassert>

#include "vec2d.h"
#include "vehicle.h"

namespace tom {

Environmental::IdType Environmental::global_id_counter = 1;

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

void Lifespan::expire() noexcept
{
    expire(false);
}

void Lifespan::expire(bool force) noexcept
{
    if (force) {
        remaining_ticks = 0;
    } else if (!unlimited_) {
        remaining_ticks = 0;
    }
}

[[nodiscard]] bool Lifespan::is_expired() const noexcept
{
    return !unlimited_ && remaining_ticks <= 0;
}

Lifespan::~Lifespan() = default;

[[nodiscard]] double Food::get_nutrition() const noexcept
{
    return nutrition;
}

void Food::update() noexcept
{
    lifespan.update();
}

bool Environmental::is_expired() const noexcept
{
    return lifespan.is_expired();
}

Environmental::Environmental(Vec2D const& pos, Lifespan ls) noexcept
    : id(next_id()), position(pos), lifespan(ls)
{
}

[[nodiscard]] Vec2D const& Environmental::get_position() const noexcept
{
    return position;
}

void Food::consume(Vehicle& consumer) noexcept
{
    consumer.health += nutrition;
    lifespan.expire();
}

Vec2D const& Food::get_position() const noexcept
{
    return position;
}

Food::Food() noexcept
    : Environmental(Vec2D::random(100), Lifespan::random(750, 1500)),
      nutrition(5.0)
{
}

Food::Food(Vec2D const& pos) noexcept
    : Environmental(pos, Lifespan::random(750, 1500)), nutrition(5.0)
{
}

Food::Food(Vec2D const& pos, double nutrition) noexcept
    : Environmental(pos, Lifespan::random(750, 1500)), nutrition(nutrition)
{
}

}  // namespace tom