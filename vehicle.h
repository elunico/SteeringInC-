#ifndef VEHICLE_H
#define VEHICLE_H

#include <optional>
#include <type_traits>
#include <vector>
#include "dna.h"
#include "utils.h"
#include "vec2d.h"
#include "world.h"

namespace tom {

template <typename T, typename U>
bool is_equal([[maybe_unused]] T* t, [[maybe_unused]] U* u)
{
    return false;
}

template <typename T>
bool is_equal(T* t, T* u)
{
    return t == u;
}

class Vehicle {
   public:
    using IdType   = World::VehicleIdType;
    using Vehicles = std::vector<Vehicle*>;
    using Foods    = std::vector<Food*>;

    explicit Vehicle(Vec2D const& position);
    Vehicle();

    [[nodiscard]] double       get_health() const;
    [[nodiscard]] int          get_age() const;
    [[nodiscard]] double       get_fitness() const;
    [[nodiscard]] DNA const&   get_dna() const;
    [[nodiscard]] Vec2D const& get_position() const;
    [[nodiscard]] Vec2D const& get_velocity() const;
    [[nodiscard]] int          get_generation() const;
    [[nodiscard]] Vec2D const& get_acceleration() const;
    [[nodiscard]] Vehicle&     last_sought_vehicle() const;
    [[nodiscard]] Vehicle&     last_sought_vehicle(double& record) const;
    [[nodiscard]] Food&        last_sought_food() const;
    [[nodiscard]] IdType       get_last_sought_vehicle_id() const;
    [[nodiscard]] bool         is_verbose() const;
    [[nodiscard]] bool         can_see(Vec2D const& target) const;
    [[nodiscard]] bool         can_touch(Vec2D const& target) const;
    [[nodiscard]] bool         can_see(double distance) const;
    [[nodiscard]] bool         can_touch(double distance) const;
    [[nodiscard]] bool         is_dead() const;
    void                       update();
    void                       kill();
    void                       avoid_edges();
    void                       behaviors(std::vector<Vehicle>& out_offspring,
                                         Vehicles&             vehicles,
                                         Foods&                food_positions);

    template <Positionable T>
    T* find_nearest(std::vector<T*>& items, double& out_distance)
    {
        T*     nearest = nullptr;
        double record  = std::numeric_limits<double>::infinity();

        for (auto& item : items) {
            // if constexpr (std::is_same_v<std::remove_cvref<decltype(*this)>,
            // T>) {
            if (is_equal(item, this))  // if (item == this)
                continue;
            // }
            double distance = position.distance_to(item->position);
            if (distance < record) {
                record  = distance;
                nearest = item;
            }
        }

        out_distance = record;
        return nearest;
    }

   private:
    static IdType global_id_counter;
    void          seek_for_eat(Food* target, double record);
    void          flee_poison(Food const* target, double record);
    void          seek_for_malice(Vehicle* target, double record);
    void          seek_for_altruism(Vehicle* target, double record);
    void          seek_for_reproduction(std::vector<Vehicle>& out_offspring,
                                        Vehicle*              target,
                                        double                record);
    Vec2D         seek(Vec2D const& target) const;
    Food&         last_sought_food(double& record) const;
    void          food_behaviors(Foods& food_positions);
    void          check_sought_vehicle();
    void          check_sought_food();
    void          vehicle_behaviors(std::vector<Vehicle>&  out_offspring,
                                    std::vector<Vehicle*>& vehicles);
    void try_explosion(std::vector<Vehicle>& out_offspring, Vehicles& vehicles);
    void apply_force(Vec2D force, bool unlimited = false);

    template <typename Getter>
    void check_helper(IdType& sought_id, Getter getter)
    {
        // if their last sought vehicle is dead or out of range, reset it
        // so they can seek a new one
        if (sought_id != 0) {
            if (!world->knows_vehicle(sought_id)) {
                sought_id = 0;
                return;
            }
            if (auto d = position.distance_to(getter().get_position());
                d > dna.perception_radius) {
                sought_id = 0;
            }
        }
    }

    World* world                        = nullptr;
    double health                       = 20.0;
    int    age                          = 0;
    double mass                         = 1.0;
    int    time_since_last_reproduction = -1;
    int    generation                   = 0;
    DNA    dna;

    Vec2D position;
    Vec2D velocity;
    Vec2D acceleration{};

   public:
    IdType            id;
    IdType            last_sought_vehicle_id = 0;
    World::FoodIdType last_sought_food_id    = 0;
    bool              verbose                = false;
    bool              highlighted            = false;

    friend struct World;
    friend struct Food;
};

}  // namespace tom

#endif  // VEHICLE_H
