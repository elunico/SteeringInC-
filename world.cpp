#include "world.h"
#include <unistd.h>
#include <csignal>
#include <iostream>
#include "food.h"
#include "irenderer.h"
#include "quadtree.h"
#include "rectangle.h"
#include "utils.h"
#include "vehicle.h"

bool World::gameRunning        = true;
bool World::isPaused           = false;
bool World::showSoughtVehicles = false;
bool World::killMode           = false;
int  World::killRadius         = 100;

#ifdef DEFLT_NO_QT
bool World::useQuadtree = false;
#else
bool World::useQuadtree = true;
#endif

std::string quadtreeMessage()
{
    if (World::useQuadtree) {
        return "[QT ON]";
    } else {
        return "[PLAIN]";
    }
}

World::World(long seed, int width, int height)
    : seed(seed), width(width), height(height)
{
    signal(SIGINT, stopRunning);
}

void World::addVehicle(Vehicle&& vehicle)
{
    vehicle.world        = this;
    vehicles[vehicle.id] = std::move(vehicle);
}

void World::addVehicle(Vec2D const& position, DNA const& dna)
{
    Vehicle v(position);
    v.dna = dna;
    addVehicle(std::move(v));
}

void World::addAllVehicles(std::vector<Vehicle>&& newVehicles)
{
    for (auto& v : newVehicles) {
        v.world = this;
    }
    for (auto& v : newVehicles) {
        vehicles[v.id] = std::move(v);
    }
}

Vec2D World::randomPosition() const
{
    return {randomInRange(0, width), randomInRange(0, height)};
}

Food const& World::newFood()
{
    Vec2D foodPosition(randomPosition());
    food.push_back(Food{foodPosition});
    return food.back();
}

auto World::pruneDeadVehicles() -> decltype(vehicles)::size_type
{
    auto const initialSize = vehicles.size();

    for (auto& [id, v] : vehicles) {
        if (v.lastSoughtVehicle != 0 &&
            vehicles[v.lastSoughtVehicle].isDead()) {
            v.lastSoughtVehicle = 0;
        }
    }

    std::erase_if(vehicles, [this](auto& p) {
        auto& v = p.second;
        if (v.isDead()) {
            deadCounter++;
            return true;
        }
        return false;
    });
    return (initialSize - vehicles.size());
}

auto World::pruneEatenFood() -> decltype(food)::size_type
{
    auto initialSize = food.size();
    food.erase(std::remove_if(food.begin(), food.end(),
                              [](Food const& f) { return f.wasEaten; }),
               food.end());
    return (initialSize - food.size());
}

std::chrono::duration<std::chrono::seconds::rep> World::elapsedTime() const
{
    if (gameRunning) {
        return std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - startTime);
    }
    return std::chrono::duration_cast<std::chrono::seconds>(endTime -
                                                            startTime);
}

void World::setup(int vehicleCount, int foodCount)
{
    for (int i = 0; i < vehicleCount; ++i) {
        Vec2D pos = randomPosition();
        createVehicle(pos);
    }

    for (int i = 0; i < foodCount; ++i) {
        newFood();
    }
}

double World::tps() const
{
    using namespace std::chrono;
    auto now      = steady_clock::now();
    auto duration = duration_cast<seconds>(now - startTime).count();
    if (duration == 0) {
        return static_cast<double>(tickCounter);
    }
    return static_cast<double>(tickCounter) / static_cast<double>(duration);
}

std::stringstream World::infoStream() const
{
    std::stringstream ss;
    ss << "(World: [" << width << "x" << height << "] " << quadtreeMessage()
       << " seed: " << seed << ") ";

    ss << "Vehicles: " << vehicles.size() << " | Food: " << food.size()
       << " | Dead Vehicles: " << deadCounter
       << " | Born Vehicles: " << bornCounter << " | Oldest Vehicle: " << maxAge
       << " | Time Elapsed: "
       << std::chrono::duration_cast<std::chrono::seconds>(
              std::chrono::steady_clock::now() - startTime)
              .count()
       << "s"
       << " | Tick: " << tickCounter << " | TPS: " << tps();
    if (World::killMode) {
        ss << "\n[KILL MODE ON (Radius: " << World::killRadius << ")] ";
    }
    return ss;
}

void World::run(IRenderer* renderer)
{
    startTime = std::chrono::steady_clock::now();
    while (gameRunning) {
        if (!isPaused) {
            if (!tick()) {
                gameRunning = false;
                break;
            }
        }
        renderer->render();
    }
    endTime = std::chrono::steady_clock::now();
}

bool World::tick()
{
    static std::vector<Vehicle> offspring;

    if (rand() % 100 < FOOD_PCT_CHANCE && food.size() < MAX_FOOD) {
        newFood();
    }

    // build quadtree for spatial partitioning (optimize neighbor searches)
    // for food and vehicles
    std::vector<Vehicle*>        neighbors;
    std::vector<Food*>           foodNeighbors;
    QuadTree<Food, Rectangle>    foodTree;
    QuadTree<Vehicle, Rectangle> vehicleTree;
    if (useQuadtree) {
        foodTree = QuadTree<Food, Rectangle>{
            Rectangle{Vec2D{0, 0}, Vec2D{static_cast<double>(width),
                                         static_cast<double>(height)}},
            4};

        vehicleTree = QuadTree<Vehicle, Rectangle>{
            Rectangle{Vec2D{0, 0}, Vec2D{static_cast<double>(width),
                                         static_cast<double>(height)}},
            4};

        for (auto& f : food) {
            foodTree.insert(&f);
        }
        for (auto& [id, v] : vehicles) {
            v.highlighted = false;
            vehicleTree.insert(&v);
        }
    } else {
        for (auto& [id, v] : vehicles) {
            v.highlighted = false;
            neighbors.push_back(&v);
        }
        for (auto& f : food) {
            foodNeighbors.push_back(&f);
        }
    }

    for (auto& food : food) {
        food.update();
    }

    for (auto& [id, vehicle] : vehicles) {
        if (useQuadtree) {
            neighbors     = vehicleTree.query(Rectangle{
                Vec2D{vehicle.position.x - vehicle.dna.perceptionRadius,
                      vehicle.position.y - vehicle.dna.perceptionRadius},
                Vec2D{vehicle.position.x + vehicle.dna.perceptionRadius,
                      vehicle.position.y + vehicle.dna.perceptionRadius}});
            foodNeighbors = foodTree.query(Rectangle{
                Vec2D{vehicle.position.x - vehicle.dna.perceptionRadius,
                      vehicle.position.y - vehicle.dna.perceptionRadius},
                Vec2D{vehicle.position.x + vehicle.dna.perceptionRadius,
                      vehicle.position.y + vehicle.dna.perceptionRadius}});
        }

        if (auto child = vehicle.behaviors(neighbors, foodNeighbors);
            child.has_value()) {
            offspring.push_back(std::move(child.value()));
        }
        vehicle.update();
        vehicle.avoidEdges();
    }

    pruneDeadVehicles();
    pruneEatenFood();

    // Add offspring to the
    addAllVehicles(std::move(offspring));
    // static_assert(std::is_trivially_destructible<Vehicle>::value,
    // "Vehicle must be trivially destructible for clear()");
    offspring.clear();
    // usleep(10000);

    tickCounter++;
    return !vehicles.empty();
}

Vehicle& World::createVehicle(Vec2D const& position)
{
    Vehicle v(position);
    v.world        = this;
    vehicles[v.id] = std::move(v);
    return vehicles[v.id];
}

void World::clearVerboseVehicles()
{
    for (auto& [id, v] : vehicles) {
        v.verbose = false;
    }
}

World::~World()
{
    // World does not own renderer, so it should not be deleted here
    // delete renderer;
}