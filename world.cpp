#include "world.h"
#include <unistd.h>
#include <csignal>
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
double const World::edge_threshold       = 25.0;

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

Food const& World::new_food()
{
    Vec2D food_position(random_position(edge_threshold));
    food.push_back(Food{food_position});
    return food.back();
}

auto World::prune_dead_vehicles() -> typename decltype(vehicles)::size_type
{
    auto const initial_size = vehicles.size();

    for (auto& [id, v] : vehicles) {
        if (v.last_sought_vehicle != 0 &&
            vehicles[v.last_sought_vehicle].is_dead()) {
            v.last_sought_vehicle = 0;
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
    food.erase(std::remove_if(food.begin(), food.end(),
                              [](Food const& f) { return f.was_eaten; }),
               food.end());
    return (initial_size - food.size());
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
        new_food();
    }
}

double World::tps() const
{
    using namespace std::chrono;
    auto now      = steady_clock::now();
    auto duration = duration_cast<seconds>(now - start_time).count();
    if (duration == 0) {
        return static_cast<double>(tick_counter);
    }
    return static_cast<double>(tick_counter) / static_cast<double>(duration);
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

void World::run(IRenderer* renderer)
{
    start_time = std::chrono::steady_clock::now();
    while (game_running) {
        if (!is_paused) {
            if (!tick()) {
                game_running = false;
                break;
            }
        }
        renderer->render();
    }
    end_time = std::chrono::steady_clock::now();
}

bool World::tick()
{
    static std::vector<Vehicle> offspring;

    if (rand() % 100 < FOOD_PCT_CHANCE && food.size() < MAX_FOOD) {
        new_food();
    }

    // build quadtree for spatial partitioning (optimize neighbor searches)
    // for food and vehicles
    std::vector<Vehicle*> neighbors;
    std::vector<Food*>    food_neighbors;

    for (auto& [id, v] : vehicles) {
        v.highlighted = false;
        neighbors.push_back(&v);
    }

    for (auto& food : food) {
        food_neighbors.push_back(&food);
        food.update();
    }

    for (auto& [id, vehicle] : vehicles) {
        if (auto child = vehicle.behaviors(neighbors, food_neighbors);
            child.has_value()) {
            offspring.push_back(std::move(child.value()));
        }
        vehicle.update();
        vehicle.avoid_edges();
    }

    prune_dead_vehicles();
    prune_eaten_food();

    // Add offspring to the
    add_all_vehicles(std::move(offspring));
    // static_assert(std::is_trivially_destructible<Vehicle>::value,
    // "Vehicle must be trivially destructible for clear()");
    offspring.clear();
    // usleep(10000);

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