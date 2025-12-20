#ifndef WORLD_H
#define WORLD_H

#include <unistd.h>
#include <chrono>
#include <functional>
#include <mutex>
#include <ostream>
#include <queue>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <vector>
#include "cyclic_num.h"
#include "dna.h"

#include "enumeration.h"
#include "irenderer.h"
#include "utils.h"
#include "vec2d.h"

namespace tom {

class Vehicle;
struct Food;

template <typename Callable, typename... Args>
concept CallableWith = requires(Callable c, Args... args) { c(args...); };

struct World {
    struct ViewMode : Enumeration<int> {
        const static ViewMode PLAIN;
        const static ViewMode FOOD_SEEKING;
        const static ViewMode VEHICLE_SEEKING;

        constexpr ViewMode(int v) : Enumeration(v)
        {
        }

        constexpr ViewMode(ViewMode const& other) = default;
    };

    struct InteractMode : Enumeration<int> {
        const static InteractMode NONE;
        const static InteractMode FEED;
        const static InteractMode KILL;

        constexpr InteractMode(int v) : Enumeration(v)
        {
        }

        constexpr InteractMode(InteractMode const& other)
            : Enumeration(other.value)
        {
        }
    };

    static constexpr int target_tps = 90;
    static bool          game_running;
    static bool          is_paused;
    static ViewMode      view_mode;
    static InteractMode  interact_mode;
    static int           kill_radius;
    static double        edge_threshold;
    static bool          was_interrupted;

    static constexpr int day_tick_length() noexcept
    {
        constexpr auto l = day_night_cycle_length * (6.0 / 10.0);
        static_assert(l != 0 && l != day_night_cycle_length,
                      "Total length too short!");
        return l;
    }

    static constexpr int night_tick_length() noexcept
    {
        constexpr auto l = day_night_cycle_length * (4.0 / 10.0);
        static_assert(l != 0 && l != day_night_cycle_length,
                      "Total length too short!");
        return l;
    }

    /*                                              1 minute days */
    static constexpr int day_night_cycle_length = target_tps * 60;

    using VehicleIdType = unsigned long;
    using FoodIdType    = unsigned long;
    using Foods         = std::unordered_map<FoodIdType, Food>;
    using Vehicles      = std::unordered_map<VehicleIdType, Vehicle>;
    using Clock         = std::chrono::steady_clock;
    using Duration      = Clock::duration;
    using TimePoint     = Clock::time_point;
    using MutexType     = std::mutex;
    using LockType      = std::scoped_lock<MutexType>;

    static constexpr Duration one_tick{Duration::period::den / target_tps};

    long                                       seed;
    int                                        width;
    int                                        height;
    std::unordered_map<VehicleIdType, Vehicle> vehicles;
    std::unordered_map<FoodIdType, Food>       food;

    // some things must wait until the end of the tick
    // like pushing back to the list of vehicles etc.
    std::queue<std::function<void(World*)>> actions;
    int                                     feed_count{};
    int                                     dead_counter    = 0;
    int                                     born_counter    = 0;
    int                                     tick_counter    = 0;
    int                                     max_age         = 0;
    unsigned int                            max_food        = 500;
    double                                  food_pct_chance = 5.0;
    std::chrono::steady_clock::time_point   start_time;
    std::chrono::steady_clock::time_point   end_time;
    cyclic<decltype(tick_counter), day_night_cycle_length> daytime;

    MutexType prune_mutex;

    static void stop_running(int)
    {
        output("Interrupting world...");
        game_running    = false;
        was_interrupted = true;
    }

    World(long seed, int width, int height);

    /**
        Perform an action between ticks.

        Any callable passed to this function must have the signature void(World
       \*)

        These functions are called and passed this after the conclusion of the
       tick during which they are queued but before the start of the subsequent
       tick

        This method is good for delaying things that cannot happen concurrent
       with the world update such as increasing the list of Food or Vehicles
       etc.

       @param c A callable object with signature auto c(World *) -> void;
    */
    template <CallableWith<World*> Callable>
    void delay(Callable c)
    {
        actions.push(c);
    }

    void add_vehicle(Vehicle&& vehicle);

    void add_vehicle(Vec2D const& position, DNA const& dna);

    void add_all_vehicles(std::vector<Vehicle>&& new_vehicles);

    [[nodiscard]] Vec2D rand_pos_in_bounds(double margin = 0.0) const;

    Food const& new_random_food();

    Food& new_food(Vec2D food_position, double nutrition);

    void new_many_food(Vec2D const& position, int count, double nutrition);

    Food const& new_food(double nutrition);

    auto prune_dead_vehicles() -> typename decltype(vehicles)::size_type;

    auto prune_eaten_food() -> typename decltype(food)::size_type;

    [[nodiscard]] bool is_day() const noexcept;

    [[nodiscard]] bool is_night() const noexcept;

    [[nodiscard]] bool knows_vehicle(VehicleIdType id) const;

    [[nodiscard]] bool knows_food(FoodIdType id) const;

    [[nodiscard]] auto elapsed_time() const -> Duration;

    void populate_world(int vehicle_count, int food_count);

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

    void check_time_of_day();

    bool tick();

    Vehicle& create_vehicle(Vec2D const& position);

    void clear_verbose_vehicles();

    ~World();

   private:
    double current_tps{};

    Food& new_food_impl(Vec2D food_position, double nutrition);

    void food_tick(Foods& food_neighbors);

    void vehicle_tick(Vehicles& neighbors, Foods& food_neighbors);

    void process_events();
};

#ifdef NO_TPS_LIMIT

constexpr inline void tps_target_wait(auto const&...)
{
    // do nothing
}

#else

template <typename Clock = World::Clock>
inline void tps_target_wait(typename Clock::time_point const& start_time,
                            typename Clock::duration const&   goal)
{
    // auto const tick_duration = Clock::now() - start_time;
    // if (tick_duration < goal) {
    //     auto sleep_duration = goal - tick_duration;
    //     std::this_thread::sleep_for(sleep_duration);
    // }

    std::this_thread::sleep_until(start_time + goal);
}
#endif

std::ostream& operator<<(std::ostream& os, World const& world);

}  // namespace tom
#endif  // WORLD_H
