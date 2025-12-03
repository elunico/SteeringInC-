#include "world.h"
#include <unistd.h>
#include <cassert>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include "food.h"
#include "irenderer.h"
#include "utils.h"
#include "vehicle.h"

bool         World::game_running         = true;
bool         World::is_paused            = false;
bool         World::show_sought_vehicles = false;
bool         World::kill_mode            = false;
int          World::kill_radius          = 100;
int          World::target_tps           = 120;
double const World::edge_threshold       = 25.0;

#define POISON_CHANCE 0.1

World::World(long seed, int width, int height)
    : seed(seed), width(width), height(height)
{
    signal(SIGINT, stop_running);
}

void World::add_vehicle(Vehicle&& vehicle)
{
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
        vehicles[v.id] = std::move(v);
    }
}

Vec2D World::random_position(double margin) const
{
    return {random_in_range(margin, width - margin),
            random_in_range(margin, height - margin)};
}

Food const& World::new_random_food()
{
    return new_food(random_in_range(0, 1) < POISON_CHANCE ? -10.0 : 5.0);
}

Food const& World::new_food(double nutrition)
{
    Vec2D food_position(random_position(edge_threshold));
    auto  id    = Food::next_id();
    Food& f     = food[id];
    f.position  = food_position;
    f.nutrition = nutrition;
    f.id        = id;
    return food[id];
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
                      auto& f = p.second;
                      return f.consumed;
                  }),
        food.end();
    return (initial_size - food.size());
}

bool World::knows_vehicle(Vehicle::IdType id) const
{
    return vehicles.contains(id);
}

bool World::knows_food(Food::IdType id) const
{
    return food.contains(id);
}

std::chrono::duration<std::chrono::seconds::rep> World::elapsed_time() const
{
    if (game_running) {
        return std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - start_time);
    }
    return std::chrono::duration_cast<std::chrono::seconds>(end_time -
                                                            start_time);
}

void World::setup(int vehicle_count, int food_count)
{
    for (int i = 0; i < vehicle_count; ++i) {
        Vec2D pos = random_position();
        create_vehicle(pos);
    }

    for (int i = 0; i < food_count; ++i) {
        new_random_food();
    }
}

double World::tps() const
{
    // using namespace std::chrono;
    // auto now      = steady_clock::now();
    // auto duration = duration_cast<seconds>(now - start_time).count();
    // if (duration == 0) {
    //     return static_cast<double>(tick_counter);
    // }
    // return static_cast<double>(tick_counter) / static_cast<double>(duration);
    return current_tps;
}

std::stringstream World::info_stream() const
{
    std::stringstream ss;
    ss << "(World: [" << width << "x" << height << "] "
       << " seed: " << seed << ") ";

    ss << "Vehicles: " << vehicles.size() << " | Food: " << food.size()
       << " | Dead Vehicles: " << dead_counter
       << " | Born Vehicles: " << born_counter
       << " | Oldest Vehicle: " << max_age << " | Time Elapsed: "
       << std::chrono::duration_cast<std::chrono::seconds>(
              std::chrono::steady_clock::now() - start_time)
              .count()
       << "s"
       << " | Tick: " << tick_counter << " | TPS: " << tps();
    if (World::kill_mode) {
        ss << "\n[KILL MODE ON (Radius: " << World::kill_radius << ")] ";
    }
    return ss;
}

long double update_tps_from_tick_duration(
    std::chrono::steady_clock::time_point const tick_start,
    std::chrono::steady_clock::time_point const tick_end)
{
    return 1e6l / std::chrono::duration_cast<std::chrono::microseconds>( // NOLINT(*-narrowing-conversions)
                      tick_end - tick_start)
                      .count();
}

void World::run(IRenderer* renderer, int target_tps)
{
    start_time = std::chrono::steady_clock::now();
    while (game_running) {
        // if (tick_counter == target_tps) {
        //     std::cout << "Completed " << target_tps << " ticks in "
        //               <<
        //               std::chrono::duration_cast<std::chrono::milliseconds>(
        //                      std::chrono::steady_clock::now() - start_time)
        //                      .count()
        //               << " ms\n";
        //     game_running = false;
        //     break;
        // }
        auto tick_start = std::chrono::steady_clock::now();
        if (!is_paused) {
            if (!tick()) {
                game_running = false;
                break;
            }
        }
        renderer->render();
        tps_target_wait(tick_start, target_tps);
        if (tick_counter % target_tps == 0) {
            auto tick_end = std::chrono::steady_clock::now();
            current_tps   = update_tps_from_tick_duration(tick_start, tick_end);
        }
    }
    end_time = std::chrono::steady_clock::now();
}

void World::food_tick(std::vector<Food*>& food_neighbors)
{
    if (rand() % 100 < FOOD_PCT_CHANCE && food.size() < MAX_FOOD) {
        new_random_food();
    }

    for (auto& [id, food] : food) {
        food.update();
    }

    prune_eaten_food();

    food_neighbors.reserve(food.size());
    for (auto& [id, food] : food) {
        food_neighbors.push_back(&food);
    }
}

void World::vehicle_tick(std::vector<Vehicle>&  offspring,
                         std::vector<Vehicle*>& neighbors,
                         std::vector<Food*>&    food_neighbors)
{
    prune_dead_vehicles();

    neighbors.reserve(vehicles.size());
    for (auto& [id, v] : vehicles) {
        v.highlighted = false;
        neighbors.push_back(&v);
    }

    for (auto& [id, vehicle] : vehicles) {
        if (auto child = vehicle.behaviors(neighbors, food_neighbors);
            child.has_value()) {
            offspring.push_back(std::move(child.value()));
        }
        vehicle.update();
        vehicle.avoid_edges();
    }

    // Add offspring to the
    add_all_vehicles(std::move(offspring));
    // static_assert(std::is_trivially_destructible<Vehicle>::value,
    // "Vehicle must be trivially destructible for clear()");
    offspring.clear();
}

bool World::tick()
{
    static std::vector<Vehicle> offspring;

    static std::vector<Vehicle*> neighbors;
    static std::vector<Food*>    food_neighbors;

    food_tick(food_neighbors);

    vehicle_tick(offspring, neighbors, food_neighbors);
    // usleep(10000);

    neighbors.clear();
    food_neighbors.clear();

    static_assert(
        std::is_trivially_destructible_v<decltype(neighbors)::value_type>);

    tick_counter++;
    return !vehicles.empty();
}

Vehicle& World::create_vehicle(Vec2D const& position)
{
    Vehicle v(position);
    v.world        = this;
    vehicles[v.id] = std::move(v);
    return vehicles[v.id];
}

void World::clear_verbose_vehicles()
{
    for (auto& [id, v] : vehicles) {
        v.verbose = false;
    }
}

World::~World()
{
    // World does not own renderer, so it should not be deleted here
    // delete renderer;
}