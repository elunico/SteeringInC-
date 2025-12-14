#include "world.h"
#include <unistd.h>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <ranges>
#include <type_traits>
#include <vector>

#include "food.h"
#include "irenderer.h"
#include "utils.h"
#include "vec2d.h"
#include "vehicle.h"

namespace tom {

World::ViewMode const World::ViewMode::PLAIN           = 0;
World::ViewMode const World::ViewMode::FOOD_SEEKING    = 1;
World::ViewMode const World::ViewMode::VEHICLE_SEEKING = 2;

World::InteractMode const World::InteractMode::NONE = 0;
World::InteractMode const World::InteractMode::FEED = 1;
World::InteractMode const World::InteractMode::KILL = 2;

bool                World::game_running    = true;
bool                World::is_paused       = false;
int                 World::kill_radius     = 100;
double              World::edge_threshold  = 25.0;
bool                World::was_interrupted = false;
World::ViewMode     World::view_mode       = ViewMode::PLAIN;
World::InteractMode World::interact_mode   = InteractMode::NONE;

#define POISON_CHANCE 0.1

World::World(long seed, int width, int height)
    : seed(seed), width(width), height(height)
{
    signal(SIGINT, stop_running);
}

void World::add_vehicle(Vehicle&& vehicle)
{
    // output("adding vehicle at position: ", vehicle.get_position(), "\n");
    vehicle.world        = this;
    vehicles[vehicle.id] = std::move(vehicle);
}

void World::add_vehicle(Vec2D const& position, DNA const& dna)
{
    Vehicle v(position);
    v.dna = dna;
    add_vehicle(std::move(v));
}

void World::add_all_vehicles(std::vector<Vehicle>&& new_vehicles)
{
    for (auto& v : new_vehicles) {
        v.world = this;
    }
    for (auto& v : new_vehicles) {
        assert(!vehicles.contains(v.id));
        vehicles[v.id] = std::move(v);
    }
}

Vec2D World::rand_pos_in_bounds(double margin) const
{
    return {random_in_range(margin, width - margin),
            random_in_range(margin, height - margin)};
}

Food const& World::new_random_food()
{
    return new_food(random_in_range(0, 1) < POISON_CHANCE ? -10.0 : 5.0);
}

Food& World::new_food(Vec2D food_position, double nutrition)
{
    auto  id    = Environmental::next_id();
    Food& f     = food[id];
    f.world     = this;
    f.position  = food_position;
    f.nutrition = nutrition;
    f.id        = id;
    return food.at(id);
}

Food const& World::new_food(double nutrition)
{
    Vec2D food_position(rand_pos_in_bounds(edge_threshold));
    return new_food(food_position, nutrition);
}

auto World::prune_dead_vehicles() -> typename decltype(vehicles)::size_type
{
    auto const initial_size = vehicles.size();

    for (auto& [id, v] : vehicles) {
        if (v.last_sought_vehicle_id != 0 &&
            vehicles[v.last_sought_vehicle_id].is_dead()) {
            v.last_sought_vehicle_id = 0;
        }
    }

    std::erase_if(vehicles, [this](auto& p) {
        auto& v = p.second;
        if (v.is_dead()) {
            dead_counter++;
            return true;
        }
        return false;
    });
    return (initial_size - vehicles.size());
}

auto World::prune_eaten_food() -> decltype(food)::size_type
{
    auto initial_size = food.size();
    std::erase_if(food,
                  [](auto const& p) {
                      auto const& f = p.second;
                      return f.is_expired();
                  }),
        food.end();
    return (initial_size - food.size());
}

bool World::is_day() const noexcept
{
    return *daytime <= day_tick_length();
}

bool World::is_night() const noexcept
{
    return *daytime > day_tick_length();
}

bool World::knows_vehicle(Vehicle::IdType id) const
{
    return vehicles.contains(id);
}

bool World::knows_food(Food::IdType id) const
{
    return food.contains(id);
}

auto World::elapsed_time() const
    -> std::chrono::duration<std::chrono::seconds::rep>
{
    if (game_running) {
        return std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - start_time);
    }
    return std::chrono::duration_cast<std::chrono::seconds>(end_time -
                                                            start_time);
}

void World::populate_world(int vehicle_count, int food_count)
{
    for (int i = 0; i < vehicle_count; ++i) {
        Vec2D pos = rand_pos_in_bounds();
        create_vehicle(pos);
    }

    for (int i = 0; i < food_count; ++i) {
        new_random_food();
    }
}

double World::tps() const
{
    return current_tps;
}

std::stringstream World::info_stream() const
{
    std::stringstream ss;
    ss << "(World: [" << width << "x" << height << "] " << " seed: " << seed
       << ") ";

    ss << "Vehicles: " << vehicles.size()
       << " ; Dead Vehicles: " << dead_counter
       << " ; Born Vehicles: " << born_counter
       << " ; Oldest Vehicle: " << max_age << " | Food: " << food.size()
       << " ; Spawn Chance " << food_pct_chance << "% ; Max: " << max_food
       << " | Time Elapsed: "
       << std::chrono::duration_cast<std::chrono::seconds>(
              std::chrono::steady_clock::now() - start_time)
              .count()
       << "s"
       << " ; Tick: " << tick_counter << " ; TPS: " << tps() << " | "
       << (*daytime > (day_tick_length()) ? "Night" : "Day");
    if (interact_mode.is_set(
            static_cast<Enumeration<int>>(InteractMode::KILL))) {
        ss << "\n[KILL MODE ON (Radius: " << World::kill_radius << ")] ";
    }
    if (interact_mode.is_set(
            static_cast<Enumeration<int>>(InteractMode::FEED))) {
        ss << "\n[FEED MODE ON (Count: " << feed_count << ")] ";
    }
    return ss;
}

long double update_tps_from_tick_duration(
    std::chrono::steady_clock::time_point const tick_start,
    std::chrono::steady_clock::time_point const tick_end)
{
    return 1e6l /
           std::chrono::duration_cast<
               std::chrono::microseconds>(  // NOLINT(*-narrowing-conversions)
               tick_end - tick_start)
               .count();
}

void World::run(render::IRenderer* renderer, int target_tps)
{
    start_time = std::chrono::steady_clock::now();
    while (game_running) {
        auto tick_start = std::chrono::steady_clock::now();
        if (!is_paused) {
            if (!tick()) {
                game_running = false;
                break;
            }
        }
        renderer->render();
        if (was_interrupted) {
            renderer->terminate();
            break;
        }
        tps_target_wait(tick_start, target_tps);
        if (tick_counter % (target_tps / 2 + 1) == 0) {
            auto tick_end = std::chrono::steady_clock::now();
            current_tps   = update_tps_from_tick_duration(tick_start, tick_end);
        }
    }
    end_time = std::chrono::steady_clock::now();
}

void World::food_tick(std::unordered_map<FoodIdType, Food>& food)
{
    prune_eaten_food();

    for (auto& [id, food] : food) {
        food.update();
    }
}

void World::vehicle_tick(
    std::unordered_map<World::VehicleIdType, Vehicle>& neighbors,
    std::unordered_map<World::FoodIdType, Food>&       food_neighbors)
{
    prune_dead_vehicles();

    for (auto& [id, vehicle] : vehicles) {
        vehicle.highlighted = false;
        vehicle.behaviors(neighbors, food_neighbors);
        vehicle.update();
        vehicle.avoid_edges();
    }
}

bool World::tick()
{
    // events are adding during ticks to be processed at the next tick, but
    // they should be thought about as belonging to the world of the prior tick
    // so they must be processed before the tick starts
    process_events();
    food_tick(food);
    vehicle_tick(vehicles, food);
    tick_counter++;
    daytime++;
    return !vehicles.empty();
}

Vehicle& World::create_vehicle(Vec2D const& position)
{
    Vehicle v(position);
    v.world        = this;
    vehicles[v.id] = std::move(v);
    return vehicles.at(v.id);
}

void World::clear_verbose_vehicles()
{
    std::ranges::for_each(vehicles | std::views::values,
                          [](auto& p) { p.verbose = false; });
}

World::~World() = default;

void World::process_events()
{
    while (!actions.empty()) {
        auto f = std::move(actions.front());
        actions.pop();
        f(this);
    }
}

std::ostream& operator<<(std::ostream& os, World const& world)
{
    os << "WORLD " << world.info_stream().str();
    return os;
}
}  // namespace tom
