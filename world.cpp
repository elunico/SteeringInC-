#include "world.h"
#include <csignal>
#include "utils.h"
#include "vehicle.h"

bool World::gameRunning = true;

World::World(double width, double height) : width(width), height(height)
{
    signal(SIGINT, stopRunning);
}

void World::addVehicle(Vehicle&& vehicle)
{
    vehicle.world = this;
    vehicles.push_back(std::move(vehicle));
}

void World::addVehicle(Vec2D const& position, DNA const& dna)
{
    Vehicle v(position);
    v.dna = dna;
    addVehicle(std::move(v));
}

Vec2D const& World::newFood()
{
    Vec2D foodPosition(randomInRange(0, width), randomInRange(0, height));
    food.push_back(foodPosition);
    return food.back();
}

auto World::pruneDeadVehicles() -> typename decltype(vehicles)::size_type
{
    auto initialSize = vehicles.size();
    vehicles.erase(
        std::remove_if(vehicles.begin(), vehicles.end(),
                       [](Vehicle const& v) { return v.getHealth() <= 0; }),
        vehicles.end());
    return (initialSize - vehicles.size());
}

Vehicle& World::createVehicle(Vec2D const& position)
{
    auto& v = vehicles.emplace_back(position);
    v.world = this;
    return v;
}
