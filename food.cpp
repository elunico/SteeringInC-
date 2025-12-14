#include "food.h"
#include <cassert>

#include "utils.h"
#include "vec2d.h"
#include "vehicle.h"
#include "world.h"

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

Environmental::Environmental(World*          world,
                             Vec2D const&    pos,
                             const Lifespan& ls) noexcept
    : id(next_id()), world(world), position(pos), lifespan(ls)
{
}

[[nodiscard]] Vec2D const& Environmental::get_position() const noexcept
{
    return position;
}

bool Environmental::is_expired() const noexcept
{
    return lifespan.is_expired();
}

Environmental::IdType Environmental::global_id_counter = 1;

Food::Food() noexcept
    : Environmental(nullptr, Vec2D::random(100), Lifespan::random(750, 1500)),
      nutrition(5.0)
{
}

Food::Food(World* world, Vec2D const& pos) noexcept
    : Environmental(world, pos, Lifespan::random(750, 1500)), nutrition(5.0)
{
}

Food::Food(World*       world,
           Vec2D const& pos,
           double       nutrition,
           DNA const&   dna) noexcept
    : Environmental(world, pos, Lifespan::random(750, 1500)),
      nutrition(nutrition),
      dna{dna}
{
}

[[nodiscard]] double Food::get_nutrition() const noexcept
{
    return nutrition;
}

Vec2D const& Food::get_position() const noexcept
{
    return position;
}

void Food::update() noexcept
{
    // velocity.limit(dna.max_speed);
    position += velocity;

    if (position.x < World::edge_threshold ||
        position.x > world->width - World::edge_threshold) {
        velocity.x *= -1;
        position.x = constrain(position.x, World::edge_threshold,
                               world->width - World::edge_threshold);
    }
    if (position.y < World::edge_threshold ||
        position.y > world->height - World::edge_threshold) {
        velocity.y *= -1;
        position.y = constrain(position.y, World::edge_threshold,
                               world->height - World::edge_threshold);
    }

    if (lifespan.remaining() < 10 &&
        random_in_range(0, 1) < dna.explosion_chance) {
        world->delay([this](auto* world) { this->perform_explosion(world); });
        lifespan.expire();
    }

    if (nutrition > 0) {
        // dont let the poison food create more food
        // TODO: feels hacky, maybe subclass Environmental for poison but world
        // has only a map of Food
        if (random_in_range(0, 1) <
                (world->food_pct_chance / 100.0 / world->target_tps) &&
            world->food.size() < world->max_food) {
            world->delay([this](auto* world) { this->perform_spawn(world); });
        }
    }
    lifespan.update();
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

void Food::perform_explosion(World* world) const
{
    if (world->food.size() >= world->max_food)
        return;
    for (int i = this->dna.explosion_tries; i > 0; i--) {
        Food& f = world->new_food(this->position, this->get_nutrition());
        f.dna   = this->dna;
        f.dna.mutate();
        if (random_in_range(0, 1) < this->dna.mutation_rate) {
            f.nutrition *= -random_in_range(1.0, 3.0);
        }
    }
}

void Food::perform_spawn(World* world) const
{
    Food& f = world->new_food(position, this->get_nutrition());
    f.dna   = this->dna;
    f.dna.mutate();
    if (random_in_range(0, 1) < f.dna.mutation_rate) {
        f.nutrition *= -random_in_range(1.0, 3.0);
    }
}
}  // namespace tom
