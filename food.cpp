#include "food.h"
#include <cassert>

#include "utils.h"
#include "vec2d.h"
#include "vehicle.h"
#include "world.h"

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
    position += velocity;
    if (position.x < World::edge_threshold ||
        position.x > world->width - World::edge_threshold) {
        velocity.x *= -1;
    }
    if (position.y < World::edge_threshold ||
        position.y > world->height - World::edge_threshold) {
        velocity.y *= -1;
    }
    if (nutrition > 0) {
        // dont let the poison food create more food
        // TODO: feels hacky, maybe subclass Environmental for poison but world
        // has only a map of Food
        if (random_in_range(0, 1) <
                (world->food_pct_chance / 100.0 / world->target_tps) &&
            world->food.size() < world->max_food) {
            world->dispatch(
                Event<Food, Food>{Event<Food, Food>::Type::FOOD_CREATED,
                                  position, world, this, nullptr});
        }
    }
    lifespan.update();
}

bool Environmental::is_expired() const noexcept
{
    return lifespan.is_expired();
}

Environmental::Environmental(World*       world,
                             Vec2D const& pos,
                             Lifespan     ls) noexcept
    : id(next_id()), world(world), position(pos), lifespan(ls)
{
}

[[nodiscard]] Vec2D const& Environmental::get_position() const noexcept
{
    return position;
}

void Food::consume(Vehicle& consumer) noexcept
{
    if (is_expired()) {
        return;
    }
    consumer.health += nutrition;
    if (consumer.verbose)
        output("Was eaten by Vehicle ID: ", consumer.id,
               " at position: ", consumer.get_position(),
               " | Nutrition: ", nutrition, "\n");
    lifespan.expire();
}

Vec2D const& Food::get_position() const noexcept
{
    return position;
}

Food::Food() noexcept
    : Environmental(nullptr, Vec2D::random(100), Lifespan::random(750, 1500)),
      nutrition(5.0)
{
}

Food::Food(World* world, Vec2D const& pos) noexcept
    : Environmental(world, pos, Lifespan::random(750, 1500)), nutrition(5.0)
{
}

Food::Food(World* world, Vec2D const& pos, double nutrition) noexcept
    : Environmental(world, pos, Lifespan::random(750, 1500)),
      nutrition(nutrition)
{
}

}  // namespace tom