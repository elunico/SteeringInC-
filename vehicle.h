#ifndef VEHICLE_H
#define VEHICLE_H

#include <optional>
#include <type_traits>
#include <unordered_map>
#include "dna.h"
#include "vec2d.h"

#include "world.h"

class Vehicle {
   public:
    using IdType = World::VehicleIdType;
    Vehicle(Vec2D const& position);
    Vehicle();

    using Vehicles = std::vector<Vehicle*>;
    using Foods    = std::vector<Food*>;

    [[nodiscard]] double       get_health() const;
    [[nodiscard]] int          get_age() const;
    [[nodiscard]] double       get_fitness() const;
    [[nodiscard]] DNA const&   getDNA() const;
    [[nodiscard]] Vec2D const& get_position() const;
    [[nodiscard]] Vec2D const& get_velocity() const;
    [[nodiscard]] int          get_generation() const;
    [[nodiscard]] Vec2D const& get_acceleration() const;
    void                       update();
    void                       kill();
    bool                       isDead() const;

    [[nodiscard]] std::optional<Vehicle> behaviors(Vehicles& vehicles,
                                                   Foods&    foodPositions);
    void                                 avoidEdges();

   private:
    static IdType          globalIdCounter;
    void                   seek_for_eat(Food* target, double record);
    void                   seek_for_malice(Vehicle* target, double record);
    void                   seek_for_altruism(Vehicle* target, double record);
    Vec2D                  seek(Vec2D const& target);
    void                   food_behaviors(Foods& foodPositions);
    void                   check_sought_vehicle();
    std::optional<Vehicle> vehicle_behaviors(Vehicles& vehicles);
    void                   apply_force(Vec2D& force, bool unlimited = false);
    [[nodiscard]] std::optional<Vehicle> reproduce(Vehicle* target,
                                                   double   record);

    template <Positionable T>
    T* findNearest(std::vector<T*>& items, double& outDistance)
    {
        T*     nearest = nullptr;
        double record  = std::numeric_limits<double>::infinity();

        for (auto& item : items) {
            if constexpr (std::is_same_v<std::decay_t<decltype(*this)>, T>) {
                if (item == this)
                    continue;
            }
            double distance = position.distanceTo(item->position);
            if (distance < record) {
                record  = distance;
                nearest = item;
            }
        }

        outDistance = record;
        return nearest;
    }

    World* world                     = nullptr;
    double health                    = 20.0;
    int    age                       = 0;
    double mass                      = 1.0;
    int    timeSinceLastReproduction = -1;
    int    generation                = 0;
    DNA    dna;

    Vec2D position;
    Vec2D velocity;
    Vec2D acceleration;

   public:
    IdType id;
    IdType lastSoughtVehicle = 0;
    bool   verbose           = false;
    bool   highlighted       = false;

    friend struct World;
};

#endif  // VEHICLE_H