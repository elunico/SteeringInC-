
#include "vehicle.h"

#include <cstddef>
#include <iostream>

#include "food.h"
#include "utils.h"
#include "vec2d.h"
#include "world.h"

#define MAX_FORCE 0.45
#define MAX_HEALTH 40.0

namespace tom {

// 0 is a sentinel value meaning no vehicle sought yet
typename Vehicle::IdType Vehicle::global_id_counter = 1;

Vehicle::Vehicle() : Vehicle(Vec2D{0.0, 0.0})
{
}

Vehicle::Vehicle(Vec2D const& position)
    : position(position),
      velocity(random_in_range(0, dna.max_speed),
               random_in_range(0, dna.max_speed)),
      id(global_id_counter++)
{
    if (auto const changer = rand() % 3; changer == 0) {
        // flip x-velocity
        velocity.x *= -1;
    } else if (changer == 1) {
        // flip y-velocity
        velocity.y *= -1;
    } else {
        // do nothing
    }
}

[[nodiscard]] double Vehicle::get_health() const
{
    return health;
}

[[nodiscard]] int Vehicle::get_age() const
{
    return age;
}

[[nodiscard]] DNA const& Vehicle::get_dna() const
{
    return dna;
}

[[nodiscard]] double Vehicle::get_fitness() const
{
    return get_health() + get_age() * 0.1;
}

[[nodiscard]] Vec2D const& Vehicle::get_position() const
{
    return position;
}
[[nodiscard]] Vec2D const& Vehicle::get_velocity() const
{
    return velocity;
}
[[nodiscard]] Vec2D const& Vehicle::get_acceleration() const
{
    return acceleration;
}

[[nodiscard]] int Vehicle::get_generation() const
{
    return generation;
}

[[nodiscard]] bool Vehicle::is_verbose() const
{
    return verbose;
}

bool Vehicle::can_see(Vec2D const& target) const
{
    return can_see(position.distance_to(target));
}

bool Vehicle::can_touch(Vec2D const& target) const
{
    return can_touch(position.distance_to(target));
}

bool Vehicle::can_see(double distance) const
{
    return distance < dna.perception_radius;
}

bool Vehicle::can_touch(double distance) const
{
    return distance < dna.max_speed * 2;
}

[[nodiscard]] Vehicle::IdType Vehicle::get_last_sought_vehicle_id() const
{
    return last_sought_vehicle_id;
}

void Vehicle::food_behaviors(Foods& food_positions)
{
    check_sought_food();

    double record =
        std::numeric_limits<double>::max();  // distance to nearest food
    auto target_food = last_sought_food_id == 0
                           ? find_nearest(food_positions, record)
                           : &last_sought_food(record);

    if (target_food != nullptr) {
        if (verbose) {
            output("Seeking food with ID: ", target_food->id,
                   " at: ", target_food->get_position(), "\n");
        }
        // update the currently sought food
        last_sought_food_id = target_food->id;
        if (target_food->nutrition < 0) {
            flee_poison(target_food, record);
        } else {
            seek_for_eat(target_food, record);
        }
    }
}

[[nodiscard]] Vehicle& Vehicle::last_sought_vehicle() const
{
    double d{};
    return last_sought_vehicle(d);
}

[[nodiscard]] Vehicle& Vehicle::last_sought_vehicle(double& record) const
{
    assert(last_sought_vehicle_id != 0);
    Vehicle& v = world->vehicles.at(last_sought_vehicle_id);
    record     = position.distance_to(v.position);
    return v;
}

[[nodiscard]] Food& Vehicle::last_sought_food() const
{
    double record;
    return last_sought_food(record);
}

void Vehicle::check_sought_vehicle()
{
    // if their last sought vehicle is dead or out of range, reset it
    // so they can seek a new one
    if (last_sought_vehicle_id != 0) {
        if (!world->knows_vehicle(last_sought_vehicle_id)) {
            last_sought_vehicle_id = 0;
            return;
        }
        if (auto d = position.distance_to(last_sought_vehicle().get_position());
            d > dna.perception_radius) {
            last_sought_vehicle_id = 0;
        }
    }
}

void Vehicle::check_sought_food()
{
    // if their last sought vehicle is dead or out of range, reset it
    // so they can seek a new one
    if (last_sought_food_id != 0) {
        if (!world->knows_food(last_sought_food_id)) {
            last_sought_food_id = 0;
            return;
        }
        if (auto d = position.distance_to(last_sought_food().get_position());
            d > dna.perception_radius) {
            last_sought_food_id = 0;
        }
    }
}

void Vehicle::vehicle_behaviors(std::vector<Vehicle*>& vehicles)
{
    double record = -1.0;  // if we are pursing the same vehicle, assume we are
                           // close enough due to prior knowledge
                           // otherwise find the nearest vehicle
    auto target_vehicle = last_sought_vehicle_id == 0
                              ? find_nearest(vehicles, record)
                              : &last_sought_vehicle(record);

    // if the *nearest* vehicle is too far too see, or there is no vehicle do
    // nothing
    if (!can_see(record) || target_vehicle == nullptr) {
        return;
    }

    // update the currently sought vehicle
    last_sought_vehicle_id = target_vehicle->id;

    if (verbose) {
        target_vehicle->highlighted = true;
    }

    seek_for_malice(target_vehicle, record);
    seek_for_altruism(target_vehicle, record);
    seek_for_reproduction(target_vehicle, record);
}

void Vehicle::try_explosion(Vehicles& vehicles)
{
    if (random_in_range(0, 1) < dna.explosion_chance) {
        for (int i = 0; i < dna.explosion_tries; i++) {
            // TODO: kill should be an event so other vehicles can respond
            // should vehicles have an event handler for being killed?
            // that would be called when the event is processed?
            // maybe it should cause them to flee the exploding vehicle area?
            kill();
            // TODO: exploding is basically mating with self; ineffecient but
            // simple to implement
            world->dispatch(Event<Vehicle, Vehicle>{
                Event<Vehicle, Vehicle>::Type::VEHICLE_BORN, position, world,
                this, nullptr});
        }

        for (auto v : vehicles) {
            if (v == this) {
                continue;
            }
            double distance = position.distance_to(v->position);
            if (can_see(distance)) {
                // TODO: dispatch an event instead of directly killing and maybe
                // scale by distance?
                v->kill();
            }
        }
    }
}

void Vehicle::behaviors(Vehicles& vehicles, Foods& food_positions)
{
    food_behaviors(food_positions);

    check_sought_vehicle();

    if (health < 2.0) {
        try_explosion(vehicles);
        return;
    }

    // only search for vehicles if health is sufficient
    if (health < 10.0) {
        return;
    }

    vehicle_behaviors(vehicles);
}

void Vehicle::flee_poison(Food const* target, double record)
{
    if (can_see(record)) {
        Vec2D desired = position - target->position;
        desired.normalize();
        desired -= velocity;
        desired *= dna.max_speed;

        Vec2D steer = desired - velocity;
        steer.limit(MAX_FORCE);
        apply_force(steer);
    }
}

void Vehicle::seek_for_eat(Food* target, double record)
{
    if (can_touch(record)) {
        // Already at target; captured food
        health += 5.0;  // Increase health on reaching target
        target->consume(*this);
    } else if (can_see(record)) {
        Vec2D steer = seek(target->position);
        apply_force(steer);
    }
}

void Vehicle::seek_for_malice(Vehicle* target, double record)
{
    if (random_in_range(0, 1) < dna.malice_probability) {
        // ATTACK!
        if (can_touch(record)) {
            target->health -= dna.malice_damage;
            this->health += dna.malice_damage * 0.5;
        } else if (can_see(record)) {
            // if vehicle is far away, try to seek it
            Vec2D steer = seek(target->position);
            steer *= dna.malice_desire;
            apply_force(steer);
        }
    }
}

void Vehicle::seek_for_altruism(Vehicle* target, double record)
{
    if (health <= 5.0 || age < dna.age_of_maturity) {
        return;  // Not enough health to attempt altruism
    }

    if (random_in_range(0, 1) < dna.altruism_probability) {
        if (can_touch(record)) {
            if (random_in_range(0, 1) < dna.altruism_probability) {
                target->health += dna.altruism_heal;
                this->health -=
                    (dna.altruism_heal * 1.1);  // Slight cost to self
            }
        } else if (can_see(record)) {
            Vec2D steer = seek(target->position);
            steer *= dna.altruism_desire;
            apply_force(steer);
        }
    }
}

Vec2D Vehicle::seek(Vec2D const& target) const
{
    Vec2D desired = target - position;
    desired.normalize();
    desired -= velocity;
    return desired;
}

Food& Vehicle::last_sought_food(double& record) const
{
    assert(last_sought_food_id != 0);
    record  = std::numeric_limits<double>::max();
    auto& f = world->food.at(last_sought_food_id);
    record  = position.distance_to(f.get_position());
    return f;
}

void Vehicle::seek_for_reproduction(Vehicle* target, double record)
{
    if (age < dna.age_of_maturity) {
        return;
    }
    if ((health < dna.reproduction_cost ||
         time_since_last_reproduction < dna.reproduction_cooldown) &&
        // if recently reproduced, cannot reproduce again yet but if never
        // reproduced before, allow reproduction
        time_since_last_reproduction != -1) {
        time_since_last_reproduction++;
        return;
    }
    if (can_touch(record)) {
        // Reproduce
        health -= dna.reproduction_cost;
        time_since_last_reproduction = 0;
        // world->born_counter++;
        world->dispatch(
            Event<Vehicle, Vehicle>{Event<Vehicle, Vehicle>::Type::VEHICLE_BORN,
                                    position, world, this, target});
        // out_offspring.push_back(offspring);
    } else {
        auto steer = seek(target->position);
        apply_force(steer);
    }

    // cannot modify world's vehicle list here to avoid iterator invalidation
    // during the processing of each vehicle currently in the world
    // after all vehicles have been processed, the main loop will add the
    // offspring to the world's vehicle list
    return;
}

void Vehicle::kill()
{
    health = 0;
}

bool Vehicle::is_dead() const
{
    return health <= 0;
}

void Vehicle::avoid_edges()
{
    Vec2D steer  = position;
    bool  active = false;

    if (position.x < World::edge_threshold) {
        steer.x = world->width;
        active  = true;
    } else if (position.x > world->width - World::edge_threshold) {
        steer.x = 0;
        active  = true;
    }

    if (position.y < World::edge_threshold) {
        steer.y = world->height;
        active  = true;
    } else if (position.y > world->height - World::edge_threshold) {
        steer.y = 0;
        active  = true;
    }

    if (active) {
        steer = seek(steer);
        apply_force(steer, true);
    }
}

void Vehicle::update()
{
    age++;
    health -= 0.05;

    velocity += acceleration;
    velocity.set_mag(dna.max_speed);

    position += velocity;

    if (position.x < 0 || position.x > world->width || position.y < 0 ||
        position.y > world->height) {
        kill();
    }

    // Reset acceleration after each update
    acceleration.reset();

    // Update world's max age if necessary
    world->max_age = std::max(world->max_age, age);
    health         = std::min(health, MAX_HEALTH);
}

void Vehicle::apply_force(Vec2D force, bool unlimited)
{
    auto pforce = force / mass;
    if (!unlimited) {
        pforce.limit(MAX_FORCE);
    }
    acceleration += pforce;
}

}  // namespace tom