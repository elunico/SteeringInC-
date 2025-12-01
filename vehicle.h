#ifndef VEHICLE_H
#define VEHICLE_H

#include <optional>
#include <type_traits>
#include "dna.h"
#include "quadtree.h"
#include "vec2d.h"

#include "world.h"

class Vehicle {
   public:
    Vehicle(Vec2D const& position);

    using Vehicles = std::vector<Vehicle*>;
    using Foods    = std::vector<Food*>;

    [[nodiscard]] double       getHealth() const;
    [[nodiscard]] int          getAge() const;
    [[nodiscard]] double       getFitness() const;
    [[nodiscard]] DNA const&   getDNA() const;
    [[nodiscard]] Vec2D const& getPosition() const;
    [[nodiscard]] Vec2D const& getVelocity() const;
    [[nodiscard]] int          getGeneration() const;
    [[nodiscard]] Vec2D const& getAcceleration() const;
    void                       update();
    void                       kill();

    [[nodiscard]] std::optional<Vehicle> behaviors(Vehicles& vehicles,
                                                   Foods&    foodPositions);
    void                                 avoidEdges();

   private:
    void  behavior_eat(Food* target, double record);
    void  behavior_malice(Vehicle* target, double record);
    void  behavior_altruism(Vehicle* target, double record);
    Vec2D seek(Vec2D const& target);
    void  applyForce(Vec2D& force, bool unlimited = false);
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

    World* world;
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
    bool verbose     = false;
    bool highlighted = false;

    friend struct World;
};

#endif  // VEHICLE_H