#ifndef WORLD_H
#define WORLD_H

#include <sstream>
#include <vector>
#include "dna.h"
#include "food.h"

#include <chrono>

#include "irenderer.h"
#include "quadtree.h"
#include "rectangle.h"
#include "vec2d.h"

#define FOOD_PCT_CHANCE 20
#define MAX_FOOD 400

class Vehicle;

struct World {
    // renderer is borrowed not owned, so it must outlive the world
    IRenderer*                            renderer;
    long                                  seed;
    int                                   width;
    int                                   height;
    std::vector<Vehicle>                  vehicles;
    std::vector<Food>                     food;
    static bool                           gameRunning;
    static bool                           isPaused;
    static bool                           useQuadtree;
    static bool                           killMode;
    static int                            killRadius;
    int                                   deadCounter = 0;
    int                                   bornCounter = 0;
    int                                   tickCounter = 0;
    int                                   maxAge      = 0;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;

    static void stopRunning(int)
    {
        gameRunning = false;
    }

    static void toggleQuadtree()
    {
        useQuadtree = !useQuadtree;
    }

    World(long seed, int width, int height, IRenderer* renderer);

    void addVehicle(Vehicle&& vehicle);

    void addVehicle(Vec2D const& position, DNA const& dna);

    void addAllVehicles(std::vector<Vehicle>&& newVehicles);

    [[nodiscard]] Vec2D randomPosition() const;

    Food const& newFood();

    auto pruneDeadVehicles() -> typename decltype(vehicles)::size_type;

    auto pruneEatenFood() -> typename decltype(food)::size_type;

    [[nodiscard]] std::chrono::duration<std::chrono::seconds::rep> elapsedTime()
        const;

    void setup(int vehicleCount, int foodCount);

    [[nodiscard]] double tps() const;

    [[nodiscard]] std::stringstream infoStream() const;

    void run();

    static void pause()
    {
        isPaused = true;
    }

    static void unpause()
    {
        isPaused = false;
    }

    bool tick();

    Vehicle& createVehicle(Vec2D const& position);

    void clearVerboseVehicles();

    ~World();
};

std::string quadtreeMessage();

#endif  // WORLD_H
