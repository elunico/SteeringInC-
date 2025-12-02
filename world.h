#ifndef WORLD_H
#define WORLD_H

#include <unistd.h>
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
    static int                                   target_tps;
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

    Food const& new_random_food();

    Food const& new_food(double nutrition);

    auto prune_dead_vehicles() -> typename decltype(vehicles)::size_type;

    auto prune_eaten_food() -> typename decltype(food)::size_type;

    [[nodiscard]] std::chrono::duration<std::chrono::seconds::rep>
    elapsed_time() const;

    void setup(int vehicle_count, int food_count);

    [[nodiscard]] double tps() const;

    [[nodiscard]] std::stringstream info_stream() const;

    void run(IRenderer* renderer, int target_tps = World::target_tps);

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

   private:
    inline static void tps_target_wait(
        std::chrono::time_point<std::chrono::steady_clock> const& start_time,
        int target_tps = World::target_tps)
    {
        auto tick_time = std::chrono::steady_clock::now() - start_time;
        // TODO: targeting 60 tps results in about 27 ms per tick leading to
        // more like ~37-40 tps in practice due to processing time?
        auto target_tick_duration =
            std::chrono::milliseconds(1000 / target_tps);
        while (tick_time < target_tick_duration) {
            // sleep to maintain target fps
            usleep(1000);
            tick_time = std::chrono::steady_clock::now() - start_time;
        }
    }
};

#endif  // WORLD_H
