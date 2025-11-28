
#include "vehicle.h"
#include <iostream>
#include <string>
#include "vec2d.h"
#include "world.h"

Vehicle::Vehicle(Vec2D const& position) : position(position)
{
}

[[nodiscard]] double Vehicle::getHealth() const
{
    return health;
}

[[nodiscard]] int Vehicle::getAge() const
{
    return age;
}

[[nodiscard]] double Vehicle::getFitness() const
{
    return getHealth() + getAge() * 0.1;
}

[[nodiscard]] Vec2D const& Vehicle::getPosition() const
{
    return position;
}
[[nodiscard]] Vec2D const& Vehicle::getVelocity() const
{
    return velocity;
}
[[nodiscard]] Vec2D const& Vehicle::getAcceleration() const
{
    return acceleration;
}

void Vehicle::eat(std::vector<Vec2D>& foodPosition)
{
    for (auto const& foodPos : foodPosition) {
        if (position.distanceTo(foodPos) <= dna.perceptionRadius) {
            Vec2D steer = seek(foodPos);
            applyForce(steer);
        }
        if (position.distanceTo(foodPos) <= dna.maxSpeed) {
            // Already at target; captured food
            health += 5.0;  // Increase health on reaching target
            world->newFood();
            break;  // Eat only one food at a time
        }
    }
}

Vec2D Vehicle::seek(Vec2D const& target)
{
    Vec2D desired = target - position;
    desired.normalize();
    desired *= dna.maxSpeed;
    desired -= velocity;
    return desired;
}

[[nodiscard]] std::optional<Vehicle> Vehicle::reproduce(
    std::vector<Vehicle>& vehicles)
{
    if (age < dna.ageOfMaturity) {
        return std::nullopt;
    }
    if ((health < dna.reproductionCost ||
         timeSinceLastReproduction < dna.reproductionCooldown) &&
        // if recently reproduced, cannot reproduce again yet but if never
        // reproduced before, allow reproduction
        timeSinceLastReproduction != -1) {
        timeSinceLastReproduction++;
        return std::nullopt;
    }
    for (size_t i = 0; i < vehicles.size(); ++i) {
        if (&vehicles[i] == this)
            continue;
        if (position.distanceTo(vehicles[i].position) < dna.perceptionRadius) {
            // Close enough to reproduce
            DNA childDNA = dna.crossover(vehicles[i].dna);
            childDNA.mutate();
            Vec2D childPos = (position + vehicles[i].position) / 2;
            health -= 0.5;  // Cost of reproduction
            Vehicle offspring(childPos);
            offspring.dna             = childDNA;
            timeSinceLastReproduction = 0;
            return offspring;
        }
    }
    // cannot modify world's vehicle list here to avoid iterator invalidation
    // during the processing of each vehicle currently in the world
    // after all vehicles have been processed, the main loop will add the
    // offspring to the world's vehicle list
    return std::nullopt;
}

void Vehicle::update()
{
    age++;
    health -= 0.1;  // Decrease health over time

    velocity += acceleration;
    velocity.limit(dna.maxSpeed);

    position += velocity;

    // Reset acceleration after each update
    acceleration.reset();
}

void Vehicle::show() const
{
    // Placeholder for rendering logic
    std::cout << "Position: (" << position.x << ", " << position.y << ")"
              << std::string(65, '\x08');
    // std::cout << "Velocity: (" << velocity.x << ", " << velocity.y <<
    // ")\n"; std::cout << "Acceleration: (" << acceleration.x << ", "
    //   << acceleration.y << ")\n";
}

void Vehicle::applyForce(Vec2D const& force)
{
    acceleration += (force / mass);
}
