
#include "vehicle.h"
#include "utils.h"
#include "vec2d.h"
#include "world.h"

#define MAX_FORCE 0.1

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

void Vehicle::eat(std::vector<Food>& foodPosition)
{
    for (auto& food : foodPosition) {
        if (food.wasEaten)
            continue;
        double d = position.distanceTo(food.position);
        if (d <= dna.perceptionRadius) {
            Vec2D steer = seek(food.position);
            steer *= 1 / d;  // stronger force when closer
            applyForce(steer);
        }
        if (d <= dna.maxSpeed) {
            // Already at target; captured food
            health += 5.0;  // Increase health on reaching target
            food.markEaten();
            break;  // Eat only one food at a time
        }
    }
}

void Vehicle::align(Vehicles const& vehicles)
{
    Vec2D avgVelocity;
    int   count = 0;
    for (size_t i = 0; i < vehicles.size(); ++i) {
        if (&vehicles[i] == this)
            continue;
        double distance = position.distanceTo(vehicles[i].position);
        if (distance < dna.perceptionRadius) {
            avgVelocity += (vehicles[i].velocity * (1 / distance));
            count++;
        }
    }
    if (count > 0) {
        avgVelocity /= static_cast<double>(count);
        auto steer = seek(avgVelocity);
        steer *= dna.coherence;
        applyForce(steer);
    }
}

void Vehicle::avoid(Vehicles const& vehicles)
{
    for (size_t i = 0; i < vehicles.size(); ++i) {
        if (&vehicles[i] == this)
            continue;
        double distance = position.distanceTo(vehicles[i].position);
        if (distance < dna.perceptionRadius) {
            Vec2D steer = seek(vehicles[i].position);
            steer *= -dna.avoidance * (1 / distance);  // stronger force when
                                                       // closer
            applyForce(steer);
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
            centerOfMass += (vehicles[i].position * (1 / distance));
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

void Vehicle::attemptMalice(Vehicles& vehicles)
{
    if (health <= 5.0 || age < dna.ageOfMaturity) {
        return;  // Not enough health to attempt malice
    }
    for (size_t i = 0; i < vehicles.size(); ++i) {
        if (&vehicles[i] == this)
            continue;
        double distance = position.distanceTo(vehicles[i].position);
        if (distance < dna.perceptionRadius) {
            if (randomInRange(0, 1) < dna.maliceProbability) {
                vehicles[i].health -= dna.maliceDamage;
                this->health += dna.maliceDamage * 0.5;  // gain some health
                // log("Vehicle attacked another vehicle!");
                break;  // only attack one vehicle at a time
            }
        }
    }
}

void Vehicle::attemptAltruism(Vehicles& vehicles)
{
    if (health <= 5.0 || age < dna.ageOfMaturity) {
        return;  // Not enough health to attempt altruism
    }
    for (size_t i = 0; i < vehicles.size(); ++i) {
        if (&vehicles[i] == this)
            continue;
        double distance = position.distanceTo(vehicles[i].position);
        if (distance < dna.perceptionRadius) {
            if (randomInRange(0, 1) < dna.altruismProbability) {
                vehicles[i].health += dna.altruismHeal;
                this->health -= dna.altruismHeal;  // lose some health
                // log("Vehicle helped another vehicle!");
                break;  // only help one vehicle at a time
            }
        }
    }
}

Vec2D Vehicle::seek(Vec2D const& target)
{
    Vec2D desired = target - position;
    desired.normalize();
    // desired *= dna.maxSpeed;
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
    velocity.setMag(dna.maxSpeed);

    position += velocity;

    if (position.x < 0 || position.x > world->width || position.y < 0 ||
        position.y > world->height) {
        health = 0;  // Vehicle dies if it goes out of bounds
    }

    // Reset acceleration after each update
    acceleration.reset();
}

void Vehicle::show() const
{
    // std::cout << "Vehicle at (" << (position.x) << ", " << (position.y) << ")
    // "
    //           << "Health: " << (health) << " Age: " << age << "\n";
}

void Vehicle::applyForce(Vec2D& force)
{
    force /= mass;
    force.limit(MAX_FORCE);
    acceleration += force;
}
