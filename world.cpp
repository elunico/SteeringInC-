#include "world.h"
#include <csignal>
#include "food.h"
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

Food const& World::newFood()
{
    Vec2D foodPosition(randomInRange(0, width), randomInRange(0, height));
    food.push_back(Food{foodPosition});
    return food.back();
}

auto World::pruneDeadVehicles() -> typename decltype(vehicles)::size_type
{
    auto initialSize = vehicles.size();

    vehicles.erase(std::remove_if(vehicles.begin(), vehicles.end(),
                                  [this](Vehicle const& v) {
                                      deadCounter++;
                                      return v.getHealth() <= 0;
                                  }),
                   vehicles.end());
    return (initialSize - vehicles.size());
}

auto World::pruneEatenFood() -> typename decltype(food)::size_type
{
    auto initialSize = food.size();
    food.erase(std::remove_if(food.begin(), food.end(),
                              [](Food const& f) { return f.wasEaten; }),
               food.end());
    return (initialSize - food.size());
}

Vehicle& World::createVehicle(Vec2D const& position)
{
    auto& v = vehicles.emplace_back(position);
    v.world = this;
    return v;
}
