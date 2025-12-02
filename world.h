#ifndef WORLD_H
#define WORLD_H

#include <chrono>
#include <sstream>
#include <unordered_map>
#include <vector>
#include "dna.h"
#include "food.h"
#include "irenderer.h"
#include "vec2d.h"

#define FOOD_PCT_CHANCE 20
#define MAX_FOOD 400

class Vehicle;

struct World {
    using Vehicle_id_type = unsigned long;
    long                                         seed;
    int                                          width;
    int                                          height;
    std::unordered_map<Vehicle_id_type, Vehicle> vehicles;
    std::vector<Food>                            food;
    static bool                                  game_running;
    static bool                                  is_paused;
    static bool                                  show_sought_vehicles;
    static bool                                  kill_mode;
    static int                                   kill_radius;
    static double const                          edge_threshold;
    int                                          dead_counter = 0;
    int                                          born_counter = 0;
    int                                          tick_counter = 0;
    int                                          max_age      = 0;
    std::chrono::steady_clock::time_point        start_time;
    std::chrono::steady_clock::time_point        end_time;

    static void stop_running(int)
    {
        game_running = false;
    }

    World(long seed, int width, int height);

    void add_vehicle(Vehicle&& vehicle);

    void add_vehicle(Vec2D const& position, DNA const& dna);

    void add_all_vehicles(std::vector<Vehicle>&& new_vehicles);

    [[nodiscard]] Vec2D random_position(double margin = 0.0) const;

    Food const& new_food();

    auto prune_dead_vehicles() -> typename decltype(vehicles)::size_type;

    auto prune_eaten_food() -> typename decltype(food)::size_type;

    [[nodiscard]] std::chrono::duration<std::chrono::seconds::rep>
    elapsed_time() const;

    void setup(int vehicle_count, int food_count);

    [[nodiscard]] double tps() const;

    [[nodiscard]] std::stringstream info_stream() const;

    void run(IRenderer* renderer);

    static void pause()
    {
        is_paused = true;
    }

    static void unpause()
    {
        is_paused = false;
    }

    bool tick();

    Vehicle& create_vehicle(Vec2D const& position);

    void clear_verbose_vehicles();

    ~World();
};

#endif  // WORLD_H
