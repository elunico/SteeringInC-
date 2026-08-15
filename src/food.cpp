#include "food.h"

#include "checks.h"
#include "fooddna.h"
#include "lifespan.h"
#include "utils.h"
#include "vec2d.h"
#include "vehicle.h"
#include "world.h"

namespace tom {

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
    : Environmental(nullptr, Vec2D::random(100), IntLifespan::random(750, 1500))
{
    lifespan = IntLifespan{(int) dna.lifeticks};
}

Food::Food(World* world, Vec2D const& pos) noexcept
    : Environmental(world, pos, IntLifespan::random(750, 1500))
{
    lifespan = IntLifespan{(int) dna.lifeticks};
}

Food::Food(World* world, Vec2D const& pos, FoodDNA const& dna) noexcept
    : Environmental(world, pos, IntLifespan::random(750, 1500)), dna{dna}
{
    lifespan = IntLifespan{(int) dna.lifeticks};
}

bool Food::sees(Vec2D const& position) const noexcept
{
    auto d = Food::position.distance_to(position);
    return (d < dna.perceptionRadius);
}

void Food::try_flee(Vehicle const& source) noexcept
{
    // TODO: is this better ?give chance every second not every tick
    if (random_in_range(0, 1) < (dna.fleeChance / World::target_tps)) {
        auto force = Vec2D::flee_force(source.position, position, velocity,
                                       dna.fleeStrength);
        apply_force(force);
    }
}

void Food::apply_force(Vec2D const& force)
{
    acceleration += force;
}

void Food::expire() noexcept
{
    lifespan.expire();
}

void Food::behaviors(World::Vehicles const& vehicles)
{
    for (auto const& [id, v] : vehicles) {
        if (sees(v.position)) {
            try_flee(v);
        }
    }
}

[[nodiscard]] double Food::get_nutrition() const noexcept
{
    return dna.nutrition;
}

Vec2D const& Food::get_position() const noexcept
{
    return position;
}

void Food::avoid_edges() noexcept
{
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
}

void Food::update() noexcept
{
    velocity += acceleration;
    velocity.limit(dna.speed);
    position += velocity;
    acceleration.reset();

    avoid_edges();

    if (lifespan.remaining() < 10 &&
        random_in_range(0, 1) < dna.explosionChance) {
        world->delay([this](auto* world) { this->perform_explosion(world); });
        lifespan.expire();
        return;
    }

    if (dna.nutrition > 0) {
        // dont let the poison food create more food
        // TODO: feels hacky, maybe subclass Environmental for poison but world
        // has only a map of Food
        if (world->should_spawn_food()) {
            world->delay([this](auto* world) { this->perform_spawn(world); });
        }
    }
    lifespan.update();
    dampen_velocity();
}

void Food::dampen_velocity() {
    velocity *= (1 - velocity_dampening);
}

void Food::consume(Vehicle& consumer) noexcept
{
    if (is_expired()) {
        return;
    }
    // see FoodDNA for more
    consumer.health += (Vehicle::MAX_HEALTH * dna.nutrition) /
                       decltype(consumer.health)::tick_amount;
    if (consumer.verbose)
        output("Was eaten by Vehicle ID: ", consumer.id,
               " at position: ", consumer.get_position(),
               " | Nutrition: ", dna.nutrition, " for ",
               (Vehicle::MAX_HEALTH * dna.nutrition), "health.\n");
    lifespan.expire();
}

void Food::perform_explosion(World* world) const
{
    GUARD(world->food.size() < world->max_food);
    for (int i = this->dna.explosionCount; i > 0; i--) {
        Food& f = world->new_food(this->position, this->get_nutrition());
        f.dna   = this->dna;
        f.dna.mutate();
        if (random_in_range(0, 1) < this->dna.mutationRate) {
            f.dna.nutrition *= -random_in_range(1.0, 3.0);
        }
    }
}

void Food::perform_spawn(World* world) const
{
    Food& f = world->new_food(position, this->get_nutrition());
    f.dna   = this->dna;
    f.dna.mutate();
    if (random_in_range(0, 1) < f.dna.mutationRate) {
        f.dna.nutrition *= -random_in_range(1.0, 3.0);
    }
}
}  // namespace tom
