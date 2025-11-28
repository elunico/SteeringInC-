#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include "dna.h"
#include "vec2d.h"

class Vehicle;

struct World {
    double               width;
    double               height;
    std::vector<Vehicle> vehicles;
    std::vector<Vec2D>   food;
    std::vector<Vec2D>   poison;
    static bool          gameRunning;

    static void stopRunning(int)
    {
        gameRunning = false;
    }

    World(double width, double height);

    void addVehicle(Vehicle&& vehicle);

    void addVehicle(Vec2D const& position, DNA const& dna);

    Vec2D const& newFood();

    auto pruneDeadVehicles() -> typename decltype(vehicles)::size_type;

    Vehicle& createVehicle(Vec2D const& position);
};

#endif  // WORLD_H