#ifndef VEHICLE_H
#define VEHICLE_H

#include <optional>
#include "dna.h"
#include "vec2d.h"

#include "world.h"

class Vehicle {
   public:
    Vehicle(Vec2D const& position);

    using Vehicles = std::vector<Vehicle>;

    [[nodiscard]] double                 getHealth() const;
    [[nodiscard]] int                    getAge() const;
    [[nodiscard]] double                 getFitness() const;
    [[nodiscard]] Vec2D const&           getPosition() const;
    [[nodiscard]] Vec2D const&           getVelocity() const;
    [[nodiscard]] Vec2D const&           getAcceleration() const;
    [[nodiscard]] std::optional<Vehicle> reproduce(Vehicles& vehicles);
    void                                 eat(std::vector<Vec2D>& foodPositions);
    Vec2D                                seek(Vec2D const& target);
    void                                 update();
    void                                 show() const;
    void                                 applyForce(Vec2D const& force);

   private:
    World* world;
    double health                    = 20.0;
    int    age                       = 0;
    double mass                      = 1.0;
    int    timeSinceLastReproduction = -1;

    Vec2D position;
    Vec2D velocity;
    Vec2D acceleration;

    DNA dna;

    friend struct World;
};

#endif  // VEHICLE_H