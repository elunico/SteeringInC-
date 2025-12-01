
#include "vehicle.h"

#include <iostream>

#include "utils.h"
#include "vec2d.h"
#include "world.h"

#define MAX_FORCE 0.2
#define MAX_HEALTH 40.0

// 0 is a sentinel value meaning no vehicle sought yet
typename Vehicle::IdType Vehicle::globalIdCounter = 1;

Vehicle::Vehicle() : Vehicle(Vec2D{0.0, 0.0})
{
}
Vehicle::Vehicle(Vec2D const& position)
    : position(position),
      velocity(randomInRange(0, dna.maxSpeed), randomInRange(0, dna.maxSpeed)),
      id(globalIdCounter++)
{
    if (auto const changer = rand() % 3; changer == 0) {
        // flip x-velocity
        velocity.x *= -1;
    } else if (changer == 1) {
        // flip y-velocity
        velocity.y *= -1;
    } else {
        // do nothing
    }
}

[[nodiscard]] double Vehicle::getHealth() const
{
    return health;
}

[[nodiscard]] int Vehicle::getAge() const
{
    return age;
}

[[nodiscard]] DNA const& Vehicle::getDNA() const
{
    return dna;
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

[[nodiscard]] int Vehicle::getGeneration() const
{
    return generation;
}

[[nodiscard]] std::optional<Vehicle> Vehicle::behaviors(Vehicles& vehicles,
                                                        Foods&    foodPositions)
{
    if (lastSoughtVehicle != 0) {
        if (auto d = position.distanceTo(
                world->vehicles[lastSoughtVehicle].getPosition());
            d > dna.perceptionRadius) {
            lastSoughtVehicle = 0;
        }
    }

    double record = -1.0;  // if we dont findNearest, assume the vehicle is
                           // close enough already
    auto targetFood    = findNearest(foodPositions, record);
    auto targetVehicle = lastSoughtVehicle != 0
                             ? &world->vehicles[lastSoughtVehicle]
                             : findNearest(vehicles, record);

    if (record > dna.perceptionRadius) {
        return std::nullopt;  // Too far away to seek anything
    }

    // TODO: there is an issue with saving this pointer.
    // if offspring are created, they may invalidate this pointer. Consider
    // using IDs instead. keep all the vehicles in a hash map with unique IDs
    // and store the ID instead of pointer
    lastSoughtVehicle = targetVehicle->id;

    if (verbose) {
        if (targetVehicle != nullptr) {
            targetVehicle->highlighted = true;
        }
    }

    if (targetFood != nullptr)
        behavior_eat(targetFood, record);
    if (targetVehicle != nullptr) {
        behavior_malice(targetVehicle, record);
        behavior_altruism(targetVehicle, record);
        return reproduce(targetVehicle, record);
    } else {
        return std::nullopt;
    }
}

void Vehicle::behavior_eat(Food* target, double record)
{
    if (record <= dna.perceptionRadius) {
        Vec2D steer = seek(target->position);
        if (verbose)
            output("Applying eat force: ", steer, "\n");
        applyForce(steer);
    } else if (record <= dna.maxSpeed) {
        // Already at target; captured food
        health += 5.0;  // Increase health on reaching target
        target->markEaten();
    }
}

void Vehicle::behavior_malice(Vehicle* target, double record)
{
    if (randomInRange(0, 1) < dna.maliceProbability) {
        // ATTACK!
        if (record < dna.maxSpeed) {
            target->health -= dna.maliceDamage;
            this->health += dna.maliceDamage * 0.5;
        }
        if (record < dna.perceptionRadius) {
            // if vehicle is far away, try to seek it
            Vec2D steer = seek(target->position);
            steer *= dna.maliceDesire;
            if (verbose)
                output("Applying malice force: ", steer, "\n");
            applyForce(steer);
        }
    }
}

void Vehicle::behavior_altruism(Vehicle* target, double record)
{
    if (health <= 5.0 || age < dna.ageOfMaturity) {
        return;  // Not enough health to attempt altruism
    }

    if (randomInRange(0, 1) < dna.altruismProbability) {
        if (record < dna.maxSpeed) {
            if (randomInRange(0, 1) < dna.altruismProbability) {
                target->health += dna.altruismHeal;
                this->health -=
                    (dna.altruismHeal * 1.1);  // Slight cost to self
            }
        }
        if (record < dna.perceptionRadius) {
            Vec2D steer = seek(target->position);
            steer *= dna.altruismDesire;
            if (verbose)
                output("Applying altruism force: ", steer, "\n");
            applyForce(steer);
        }
    }
}

Vec2D Vehicle::seek(Vec2D const& target)
{
    Vec2D desired = target - position;
    desired.normalize();
    desired -= velocity;
    return desired;
}

[[nodiscard]] std::optional<Vehicle> Vehicle::reproduce(Vehicle* target,
                                                        double   record)
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
    if (record < dna.maxSpeed) {
        // Close enough to reproduce
        DNA childDNA = dna.crossover(target->dna);
        childDNA.mutate();
        Vec2D childPos = (position + target->position) / 2;
        health -= 0.5;  // Cost of reproduction
        Vehicle offspring(childPos);
        offspring.generation = std::max(generation, target->generation) + 1;
        // move away from parents slightly
        offspring.velocity = Vec2D(randomInRange(-1, 1), randomInRange(-1, 1));
        offspring.dna      = childDNA;
        timeSinceLastReproduction = 0;
        world->bornCounter++;
        return offspring;
    } else {
        auto steer = seek(target->position);
        if (verbose)
            output("Applying reproduction force: ", steer, "\n");
        applyForce(steer);
    }

    // cannot modify world's vehicle list here to avoid iterator invalidation
    // during the processing of each vehicle currently in the world
    // after all vehicles have been processed, the main loop will add the
    // offspring to the world's vehicle list
    return std::nullopt;
}

void Vehicle::kill()
{
    health = 0;
}

bool Vehicle::isDead() const
{
    return health <= 0;
}

void Vehicle::avoidEdges()
{
    static const double edgeThreshold = 25.0;
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

    position.x =
        std::clamp(position.x, edgeThreshold, world->width - edgeThreshold);
    position.y =
        std::clamp(position.y, edgeThreshold, world->height - edgeThreshold);

    if (steer.x != 0 || steer.y != 0) {
        steer = seek(steer);
        steer *= 2.0;  // stronger force to avoid edges
        if (verbose)
            output("Applying avoid edges force: ", steer, "\n");
        applyForce(steer, true);
    }
}

void Vehicle::update()
{
    age++;
    health -= 0.05;

    velocity += acceleration;
    velocity.setMag(dna.maxSpeed);

    position += velocity;

    if (position.x < 0 || position.x > world->width || position.y < 0 ||
        position.y > world->height) {
        health = 0;  // Vehicle dies if it goes out of bounds
    }

    // Reset acceleration after each update
    acceleration.reset();

    // Update world's max age if necessary
    world->maxAge = std::max(world->maxAge, age);
    health        = std::min(health, MAX_HEALTH);
}

void Vehicle::applyForce(Vec2D& force, bool unlimited)
{
    force /= mass;
    if (!unlimited) {
        force.limit(MAX_FORCE);
    }
    acceleration += force;
}
