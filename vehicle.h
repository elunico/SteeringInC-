#ifndef VEHICLE_H
#define VEHICLE_H

#include <optional>
#include <type_traits>
#include <vector>
#include "dna.h"
#include "utils.h"
#include "vec2d.h"
#include "world.h"

class Vehicle {
   public:
    using IdType   = World::Vehicle_id_type;
    using Vehicles = std::vector<Vehicle*>;
    using Foods    = std::vector<Food*>;

    static const double edge_threshold;

    Vehicle(Vec2D const& position);
    Vehicle();

    [[nodiscard]] double                 get_health() const;
    [[nodiscard]] int                    get_age() const;
    [[nodiscard]] double                 get_fitness() const;
    [[nodiscard]] DNA const&             get_dna() const;
    [[nodiscard]] Vec2D const&           get_position() const;
    [[nodiscard]] Vec2D const&           get_velocity() const;
    [[nodiscard]] int                    get_generation() const;
    [[nodiscard]] Vec2D const&           get_acceleration() const;
    [[nodiscard]] Vehicle&               last_sought_vehicle() const;
    [[nodiscard]] Food&                  last_sought_food() const;
    [[nodiscard]] IdType                 get_last_sought_vehicle_id() const;
    [[nodiscard]] bool                   is_verbose() const;
    void                                 update();
    void                                 kill();
    bool                                 is_dead() const;
    void                                 avoid_edges();
    [[nodiscard]] std::optional<Vehicle> behaviors(Vehicles& vehicles,
                                                   Foods&    food_positions);

    template <Positionable T>
    T* find_nearest(std::vector<T*>& items, double& out_distance)
    {
        T*     nearest = nullptr;
        double record  = std::numeric_limits<double>::infinity();

        for (auto& item : items) {
            if constexpr (std::is_same_v<std::decay_t<decltype(*this)>, T>) {
                if (item == this)
                    continue;
            }
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
    static IdType          global_id_counter;
    void                   seek_for_eat(Food* target, double record);
    void                   flee_poison(Food* target, double record);
    void                   seek_for_malice(Vehicle* target, double record);
    void                   seek_for_altruism(Vehicle* target, double record);
    Vec2D                  seek(Vec2D const& target);
    void                   food_behaviors(Foods& food_positions);
    void                   check_sought_vehicle();
    void                   check_sought_food();
    std::optional<Vehicle> vehicle_behaviors(Vehicles& vehicles);
    void                   apply_force(Vec2D& force, bool unlimited = false);
    [[nodiscard]] std::optional<Vehicle> reproduce(Vehicle* target,
                                                   double   record);

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
    IdType              id;
    IdType              last_sought_vehicle_id = 0;
    World::Food_id_type last_sought_food_id    = 0;
    bool                verbose                = false;
    bool                highlighted            = false;

    friend struct World;
    friend struct Food;
};

#endif  // VEHICLE_H