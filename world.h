#ifndef WORLD_H
#define WORLD_H

#include <unistd.h>
#include <chrono>
#include <ostream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include "dna.h"

#include "irenderer.h"
#include "vec2d.h"

#define FOOD_PCT_CHANCE 20
#define MAX_FOOD 400

namespace tom {

class Vehicle;
struct Food;

struct World {
    static bool   game_running;
    static bool   is_paused;
    static bool   show_sought_vehicles;
    static bool   kill_mode;
    static bool   feed_mode;
    static int    kill_radius;
    static int    target_tps;
    static double edge_threshold;
    static bool   was_interrupted;

    using VehicleIdType = unsigned long;
    using FoodIdType    = unsigned long;
    long                                       seed;
    int                                        width;
    int                                        height;
    std::unordered_map<VehicleIdType, Vehicle> vehicles;
    std::unordered_map<FoodIdType, Food>       food;
    int                                        feed_count{};
    int                                        dead_counter = 0;
    int                                        born_counter = 0;
    int                                        tick_counter = 0;
    int                                        max_age      = 0;
    std::chrono::steady_clock::time_point      start_time;
    std::chrono::steady_clock::time_point      end_time;

    static void stop_running(int)
    {
        game_running    = false;
        was_interrupted = true;
    }

    World(long seed, int width, int height);

    void add_vehicle(Vehicle&& vehicle);

    void add_vehicle(Vec2D const& position, DNA const& dna);

    void add_all_vehicles(std::vector<Vehicle>&& new_vehicles);

    [[nodiscard]] Vec2D rand_pos_in_bounds(double margin = 0.0) const;

    Food const& new_random_food();

    Food const& new_food(Vec2D food_position, double nutrition);

    Food const& new_food(double nutrition);

    auto prune_dead_vehicles() -> typename decltype(vehicles)::size_type;

    auto prune_eaten_food() -> typename decltype(food)::size_type;

    [[nodiscard]] bool knows_vehicle(VehicleIdType id) const;

    [[nodiscard]] bool knows_food(FoodIdType id) const;

    [[nodiscard]] std::chrono::duration<std::chrono::seconds::rep>
    elapsed_time() const;

    void setup(int vehicle_count, int food_count);

    [[nodiscard]] double tps() const;

    [[nodiscard]] std::stringstream info_stream() const;

    void run(render::IRenderer* renderer, int target_tps = World::target_tps);

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
    double current_tps{};

    void food_tick(std::vector<Food*>& food_neighbors);
    void vehicle_tick(std::vector<Vehicle>&  offspring,
                      std::vector<Vehicle*>& neighbors,
                      std::vector<Food*>&    food_neighbors);

#ifdef NO_TPS_LIMIT
    constexpr static void tps_target_wait(auto const&...)
    {
        // do nothing
    }

#else
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
#endif
};

std::ostream& operator<<(std::ostream& os, World const& world);

}  // namespace tom
#endif  // WORLD_H
