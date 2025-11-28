
#include "vehicle.h"
#include <curses.h>
#include <iostream>
#include <string>
#include "utils.h"
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

void Vehicle::eat(std::vector<Food>& foodPosition)
{
    for (auto& food : foodPosition) {
        if (food.wasEaten)
            continue;
        if (position.distanceTo(food.position) <= dna.perceptionRadius) {
            Vec2D steer = seek(food.position);
            applyForce(steer);
        }
        if (position.distanceTo(food.position) <= dna.maxSpeed) {
            // Already at target; captured food
            health += 5.0;  // Increase health on reaching target
            food.markEaten();
            break;  // Eat only one food at a time
        }
    }
}

void Vehicle::avoid(Vehicles const& vehicles)
{
    for (size_t i = 0; i < vehicles.size(); ++i) {
        if (&vehicles[i] == this)
            continue;
        double distance = position.distanceTo(vehicles[i].position);
        if (distance < dna.perceptionRadius && distance > 0) {
            // todo: maybe use seek()?
            Vec2D fleeVector = position - vehicles[i].position;
            fleeVector.normalize();
            fleeVector *= dna.maxSpeed;
            fleeVector -= velocity;
            fleeVector *= dna.avoidance;
            applyForce(fleeVector);
        }
    }
}

void Vehicle::cohere(Vehicles const& vehicles)
{
    Vec2D centerOfMass;
    int   count = 0;
    for (size_t i = 0; i < vehicles.size(); ++i) {
        if (&vehicles[i] == this)
            continue;
        double distance = position.distanceTo(vehicles[i].position);
        if (distance < dna.perceptionRadius) {
            centerOfMass += vehicles[i].position;
            count++;
        }
    }
    if (count > 0) {
        centerOfMass /= static_cast<double>(count);
        Vec2D steer = seek(centerOfMass);
        steer *= dna.coherence;
        applyForce(steer);
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
            // move away from parents slightly
            offspring.velocity =
                Vec2D(randomInRange(-1, 1), randomInRange(-1, 1));
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

void Vehicle::avoidEdges()
{
    static const double edgeThreshold = 10.0;
    Vec2D               steer;

    if (position.x < edgeThreshold) {
        steer.x = dna.maxSpeed;
    } else if (position.x > world->width - edgeThreshold) {
        steer.x = -dna.maxSpeed;
    }
    if (position.y < edgeThreshold) {
        steer.y = dna.maxSpeed;
    } else if (position.y > world->height - edgeThreshold) {
        steer.y = -dna.maxSpeed;
    }

    if (steer.x != 0 || steer.y != 0) {
        steer = seek(steer);
        applyForce(steer);
    }
}

void Vehicle::update()
{
    age++;
    health -= 0.1;  // Decrease health over time

    velocity += acceleration;
    velocity.limit(dna.maxSpeed);

    position += velocity;

    avoidEdges();

    // Reset acceleration after each update
    acceleration.reset();
}

void Vehicle::show() const
{
    if (health <= 5.0)
        mvprintw(static_cast<int>(position.y), static_cast<int>(position.x),
                 "X");
    else
        mvprintw(static_cast<int>(position.y), static_cast<int>(position.x),
                 "V");
    // Placeholder for rendering logic
    // std::cout << "Position: (" << position.x << ", " << position.y << ")"
    //   << std::string(65, '\x08');
    // std::cout << "Velocity: (" << velocity.x << ", " << velocity.y <<
    // ")\n"; std::cout << "Acceleration: (" << acceleration.x << ", "
    //   << acceleration.y << ")\n";
}

void Vehicle::applyForce(Vec2D const& force)
{
    acceleration += (force / mass);
}
