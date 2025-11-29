#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include "dna.h"
#include "food.h"

#include "vec2d.h"

class Vehicle;

struct World {
    long                 seed;
    int                  width;
    int                  height;
    std::vector<Vehicle> vehicles;
    std::vector<Food>    food;
    std::vector<Vec2D>   poison;
    static bool          gameRunning;
    int                  deadCounter = 0;
    int                  tickCounter = 0;

    static void stopRunning(int)
    {
        gameRunning = false;
    }

    World(long seed, int width, int height);

    Vec2D randomPosition() const;

    bool tick();

    void setup(int vehicleCount, int foodCount);

    void addVehicle(Vehicle&& vehicle);

    void addVehicle(Vec2D const& position, DNA const& dna);

    void addAllVehicles(std::vector<Vehicle>&& newVehicles);

    Food const& newFood();

    auto pruneDeadVehicles() -> typename decltype(vehicles)::size_type;

    auto pruneEatenFood() -> typename decltype(food)::size_type;

    Vehicle& createVehicle(Vec2D const& position);
};

#endif  // WORLD_H