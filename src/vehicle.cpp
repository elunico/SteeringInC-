
#include "vehicle.h"

#include <cassert>
#include <cstddef>
#include <ostream>
#include <sstream>

#include "food.h"
#include "lifespan.h"
#include "optionset.h"
#include "utils.h"
#include "vec2d.h"
#include "world.h"

namespace tom {

template <typename T, T amount>
static Lifespan<T, amount> min(Lifespan<T, amount> const& lifespan, T d)
{
    if (lifespan > d) {
        return Lifespan<T, amount>(d);
    }
    return lifespan;
}

template <typename T, T amount>
static Lifespan<T, amount> max(Lifespan<T, amount> const& lifespan, T d)
{
    if (lifespan < d) {
        return Lifespan<T, amount>(d);
    }
    return lifespan;
}

int const    Vehicle::WANDER_DISTANCE = 50;
double const Vehicle::MAX_FORCE       = 0.45;
double const Vehicle::MAX_HEALTH      = 45.0;

Vehicle::Vehicle(Vec2D const& position)
    : position(position),
      velocity(random_in_range(0, dna.max_speed),
               random_in_range(0, dna.max_speed)),
      id(global_id_counter++)
{
    if (auto const changer = random_int(1, 3) % 3; changer == 0) {
        // flip x-velocity
        velocity.x *= -1;
    } else if (changer == 1) {
        // flip y-velocity
        velocity.y *= -1;
    } else {
        // do nothing
    }

    wanderTarget = velocity.copy();
    wanderTarget.set_mag(Vehicle::WANDER_DISTANCE);
    wanderTarget += position;
}

Vehicle::Vehicle() : Vehicle(Vec2D{0.0, 0.0})
{
    // output("Default constructor called. Vehicle id is ", id, "\n");
}

[[nodiscard]] Vehicle::LifespanType Vehicle::get_health() const
{
    return health;
}

[[nodiscard]] int Vehicle::get_age() const
{
    return age;
}

[[nodiscard]] double Vehicle::get_fitness() const
{
    return get_health().remaining() + get_age() * 0.1;
}

[[nodiscard]] DNA const& Vehicle::get_dna() const
{
    return dna;
}

[[nodiscard]] Vec2D const& Vehicle::get_position() const
{
    return position;
}

[[nodiscard]] Vec2D const& Vehicle::get_velocity() const
{
    return velocity;
}

[[nodiscard]] int Vehicle::get_generation() const
{
    return generation;
}
[[nodiscard]] Vec2D const& Vehicle::get_acceleration() const
{
    return acceleration;
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

[[nodiscard]] Vehicle::IdType Vehicle::get_last_sought_vehicle_id() const
{
    return last_sought_vehicle_id;
}

[[nodiscard]] bool Vehicle::is_verbose() const
{
    return verbose;
}

bool Vehicle::can_see(Vec2D const& target) const
{
    return can_see(position.distance_to(target));
}

[[nodiscard]] bool Vehicle::will_seek_vehicle() const
{
    // during the day Vehicles only seek others if they are high enough to not
    // need food too badly when health is very low Vehicles scramble for food
    // only. at night they always want to be around others. note that this does
    // not prevent seeking food but does average velocities so food seeking will
    // be mediated by the desire to stay near others
    return is_health_pct_above(0.5) || (world && world->is_night());
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

bool Vehicle::is_dead() const
{
    return health.is_expired();
}

[[nodiscard]] double Vehicle::get_health_pct() const
{
    double remainingTicks = health.remaining();
    double totalTicks     = Vehicle::MAX_HEALTH;
    double lifePct        = remainingTicks / totalTicks;
    return lifePct;
}

[[nodiscard]] bool Vehicle::is_health_pct_above(double pct) const
{
    return get_health_pct() > pct;
}

[[nodiscard]] bool Vehicle::is_health_pct_below(double pct) const
{
    return get_health_pct() < pct;
}

static inline std::ostream& operator<<(std::ostream&          os,
                                       Vehicle::BehaviorState state)
{
    switch (state) {
        case tom::Vehicle::BehaviorState::DESPERATE:
            os << "DESPERATE";
            break;
        case tom::Vehicle::BehaviorState::OUTGOING:
            os << "OUTGOING";
            break;
        case tom::Vehicle::BehaviorState::HUNGRY:
            os << "HUNGRY";
            break;
        case tom::Vehicle::BehaviorState::UNSET:
            os << "UNSET";
            break;
        case tom::Vehicle::BehaviorState::WANDERING:
            os << "WANDERING";
            break;
    }
    return os;
}

template <typename T>
static inline std::ostream& operator<<(std::ostream&       os,
                                       OptionSet<T> const& state)
{
    os << "OptionSet(";
    auto i = state.options.cbegin();
    while (i != state.options.cend()) {
        os << *i++;
        if (i == state.options.cend())
            break;
        os << " | ";
    }
    os << ")";
    return os;
}

void Vehicle::update()
{
    if (verbose) {
        std::stringstream s;
        s << "Vehicle " << id << " health " << health.remaining()
          << " (pct=" << get_health_pct() << ")"
          << " will_seek_vehicle()=" << will_seek_vehicle() << " age " << age
          << " is_hungry()=" << is_hungry()
          << " BehaviorState=" << behavior_state;
        auto msg = s.str();
        tom::output(msg);
        tom::output(std::string(msg.size(), '\b'));
    }
    if (health == 0)
        return;

    age++;
    health--;

    if (health.is_expired()) {
        world->delay(
            [position = this->position, age = this->age](World* world) {
                world->new_food(position, age / 100.0 + 1.0);
            });
        return;
    }

    velocity += acceleration;
    velocity.limit(dna.max_speed);

    position += velocity;

    if (position.x < 0 || position.x > world->width || position.y < 0 ||
        position.y > world->height) {
        kill();
        return;
    }

    // Reset acceleration after each update
    acceleration.reset();

    avoid_edges();

    // Update world's max age if necessary
    using std::max, std::min;
    world->max_age = max(world->max_age, age);
    health         = min(health, MAX_HEALTH);
}

void Vehicle::kill()
{
    health.expire();
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

void Vehicle::wander()
{
    // this code is taken from Coding Train and Craig Reynolds
    // https://editor.p5js.org/codingtrain/sketches/VdHUvgHkm
    auto wPoint = velocity.copy();
    wPoint.set_mag(Vehicle::WANDER_DISTANCE);
    wPoint += position;
    auto offset  = 32;  // TODO: make a variable / DNA?
    auto vOffset = Vec2D::random(offset * 0.2);
    wanderTarget += vOffset;

    auto v = wanderTarget - wPoint;
    v.set_mag(Vehicle::WANDER_DISTANCE);
    wanderTarget = wPoint + v;
    auto force   = wanderTarget - position;
    // force.set_mag(MAX_FORCE);
    apply_force(force);
}

[[nodiscard]] bool Vehicle::is_hungry() const
{
    return is_health_pct_below(0.5);
}

void Vehicle::determine_behavior()
{
    behavior_state.clear();
    if (is_health_pct_below(0.1)) {
        behavior_state.set(BehaviorState::DESPERATE);
        return;
    }
    if ((is_health_pct_above(0.8)) && (world && world->is_day())) {
        behavior_state.set(BehaviorState::WANDERING);
        return;
    }
    // desperation and wandering are unique actions
    // but a vehicle may seek food or others or try to find food while
    // remaining close to others so the actions are not mutually exclusive
    if (is_hungry()) {
        behavior_state.add(BehaviorState::HUNGRY);
    }
    if (will_seek_vehicle()) {
        behavior_state.add(BehaviorState::OUTGOING);
    }
}

void Vehicle::behaviors(
    std::unordered_map<World::VehicleIdType, Vehicle>& vehicles,
    std::unordered_map<World::FoodIdType, Food>&       food_positions)
{
    check_sought_food();
    check_sought_vehicle();

    determine_behavior();

    if (behavior_state.contains(BehaviorState::DESPERATE)) {
        try_explosion();
    }

    if (behavior_state.contains(BehaviorState::WANDERING)) {
        wander();
    }

    if (behavior_state.contains(BehaviorState::HUNGRY)) {
        food_behaviors(food_positions);
    }

    if (behavior_state.contains(BehaviorState::OUTGOING)) {
        vehicle_behaviors(vehicles);
    }
}

void Vehicle::populate_in_place(typename Vehicle::IdType id,
                                World*                   world,
                                Vec2D const&             position,
                                Vec2D const&             velocity,
                                DNA const&               dna,
                                int                      generation,
                                LifespanType             health,
                                bool                     verbose)
{
    this->world      = world;
    this->health     = health;
    this->generation = generation;
    this->dna        = dna;
    this->position   = position;
    this->velocity   = velocity;
    this->id         = id;
    this->verbose    = verbose;
}

bool Vehicle::is_less_fit(Vehicle const& other) const
{
    return get_fitness() < other.get_fitness();
}
// 0 is a sentinel value meaning no vehicle sought yet
typename Vehicle::IdType Vehicle::global_id_counter = 1;

void Vehicle::seek_for_eat(Food* target, double record)
{
    if (can_touch(record)) {
        // Already at target; captured food
        // health += 50.0;  // Increase health on reaching target
        target->consume(*this);
    } else if (can_see(record)) {
        Vec2D steer = seek(target->position);
        apply_force(steer);
    }
}

void Vehicle::flee_poison(Food* target, double record)
{
    if (can_see(record)) {
        if (random_in_range(0, 1) < dna.altruism_probability) {
            target->expire();  // altruistically remove poison food
            health--;          // slight health cost to self
            return;
        }
        Vec2D steer = flee(target->position);
        apply_force(steer);
    }
}

void Vehicle::seek_for_malice(Vehicle* target, double record)
{
    if (random_in_range(0, 1) < dna.malice_probability) {
        // ATTACK!
        if (can_touch(record)) {
            target->health -= dna.malice_damage;
            this->health += dna.malice_damage;
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
                // Slight cost to self
                this->health -= (dna.altruism_heal * 1.1);
            }
        } else if (can_see(record)) {
            Vec2D steer = seek(target->position);
            steer *= dna.altruism_desire;
            apply_force(steer);
        }
    }
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
        world->delay([this, mom = this, dad = target](auto*) {
            if (mom == nullptr || dad == nullptr) {
                return;
            }
            this->perform_reproduction(*mom, *dad);
        });
    } else {
        auto steer = seek(target->position);
        apply_force(steer);
    }

    // cannot modify world's vehicle list here to avoid iterator invalidation
    // during the processing of each vehicle currently in the world
    // after all vehicles have been processed, the main loop will add the
    // offspring to the world's vehicle list
}

Vec2D Vehicle::flee(Vec2D const& target) const
{
    Vec2D desired = position - target;
    desired.set_mag(dna.max_speed);
    desired -= velocity;
    return desired;
}

Vec2D Vehicle::seek(Vec2D const& target) const
{
    Vec2D desired = target - position;
    desired.set_mag(dna.max_speed);
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

void Vehicle::food_behaviors(Foods& food_positions)
{
    // WARN: Must call check_sought_food first!
    double record      = std::numeric_limits<double>::max();
    auto*  target_food = last_sought_food_id == 0
                             ? find_nearest(food_positions, record)
                             : &last_sought_food(record);

    if (target_food != nullptr && can_see(record)) {
        // if (verbose) {
        //     output("Seeking food with ID: ", target_food->id,
        //            " at: ", target_food->get_position(), "\n");
        // }
        // update the currently sought food
        last_sought_food_id = target_food->id;
        if (target_food->get_nutrition() < 0) {
            flee_poison(target_food, record);
        } else {
            seek_for_eat(target_food, record);
        }
    }
}

void Vehicle::check_sought_vehicle()
{
    // if they are too unhealthy to seek a vehicle, they should forget
    if (!will_seek_vehicle()) {
        last_sought_vehicle_id = 0;
        return;
    }
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
        auto& f = last_sought_food();
        if (auto d = position.distance_to(f.get_position());
            d > dna.perception_radius || f.is_expired()) {
            last_sought_food_id = 0;
        }
    }
}

void Vehicle::vehicle_behaviors(Vehicles& vehicles)
{
    double record = -1.0;  // if we are pursing the same vehicle, assume we are
                           // close enough due to prior knowledge
                           // otherwise find the nearest vehicle
    auto target_vehicle = last_sought_vehicle_id == 0
                              ? find_nearest(vehicles, record)
                              : &last_sought_vehicle(record);

    // if the *nearest* vehicle is too far to see, or there is no vehicle, do
    // nothing
    if (!can_see(record) || target_vehicle == nullptr) {
        return;
    }

    // update the currently sought vehicle
    assert(target_vehicle->id != id);
    last_sought_vehicle_id = target_vehicle->id;

    if (verbose) {
        target_vehicle->highlighted = true;
    }

    if (target_vehicle->health < 5.0) {
        // vehicle could explode soon, avoid it
        Vec2D steer = flee(target_vehicle->position);
        apply_force(steer);
        return;
    }

    seek_for_malice(target_vehicle, record);
    seek_for_altruism(target_vehicle, record);
    seek_for_reproduction(target_vehicle, record);
}

void Vehicle::try_explosion()
{
    if (random_in_range(0, 1) < dna.explosion_chance) {
        kill();
        world->delay([this](auto* world) { this->perform_explosion(world); });
    }
}

void Vehicle::apply_force(Vec2D force, bool unlimited)
{
    force /= mass;
    if (!unlimited) {
        force.limit(MAX_FORCE);
    }
    acceleration += force;
}

void Vehicle::perform_reproduction(Vehicle const& mom, Vehicle const& dad) const
{
    Vec2D start_pos = mom.position;
    auto  other_pos = dad.position;
    Vec2D child_pos = midpoint(start_pos, other_pos);
    DNA   child_dna = mom.dna.crossover(dad.dna);
    child_dna.mutate();
    Vehicle offspring(child_pos);
    offspring.populate_in_place(
        Vehicle::next_id(), mom.world, child_pos, Vec2D::random(2.0), child_dna,
        std::max(mom.generation, dad.generation) + 1,
        midpoint(mom.health, dad.health).remaining() * 1.2, false);
    world->born_counter++;
    // if (mom.verbose || dad.verbose)
    // output("Adding reproduced vehicle: ", child_pos, "\n");
    world->add_vehicle(std::move(offspring));
}

void Vehicle::perform_explosion(World* world) const
{
    Vec2D start_pos = this->position;
    // Explosion-born vehicle
    unsigned long count = this->dna.explosion_tries;
    // if (this->verbose)
    // output("Adding ", count,
    //    " explosion-born vehicle around position: ", start_pos, "\n");

    for (auto& [id, v] : world->vehicles) {
        if (auto d = this->position.distance_to(v.position);
            d < this->dna.perception_radius) {
            // v.kill();
            v.health *= 0.67;
        }
    }
    std::vector children{count, Vehicle(start_pos)};
    for (unsigned long i = 0; i < count; i++) {
        Vehicle& offspring = children[i];
        offspring.position += Vec2D::random(dna.perception_radius / 2.0);
        DNA child_dna = this->dna;
        child_dna.mutate();
        // reduce the likelihood of chained explosions
        child_dna.explosion_chance /= 2;
        using std::max;

        offspring.populate_in_place(Vehicle::next_id(), this->world, start_pos,
                                    Vec2D::random(2.0), child_dna,
                                    this->generation + 1,
                                    max(this->health, 2.0), this->verbose);
        world->born_counter++;
        // if (offspring.verbose)
        //     output("Vehicle ", offspring.id,
        //            " born at position: ", offspring.position, "\n");
    }
    world->add_all_vehicles(std::move(children));
}

}  // namespace tom
