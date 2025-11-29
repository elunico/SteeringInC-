#include "world.h"
#include <unistd.h>
#include <csignal>
#include "food.h"
#include "utils.h"
#include "vehicle.h"

#define FOOD_PCT_CHANCE 20
#define MAX_FOOD 400

bool World::gameRunning = true;

World::World(long seed, int width, int height)
    : seed(seed), width(width), height(height)
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

void World::addAllVehicles(std::vector<Vehicle>&& newVehicles)
{
    for (auto& v : newVehicles) {
        v.world = this;
    }
    vehicles.insert(vehicles.end(),
                    std::make_move_iterator(newVehicles.begin()),
                    std::make_move_iterator(newVehicles.end()));
}

Vec2D World::randomPosition() const
{
    return Vec2D(randomInRange(0, width), randomInRange(0, height));
}

Food const& World::newFood()
{
    Vec2D foodPosition(randomPosition());
    food.push_back(Food{foodPosition});
    return food.back();
}

auto World::pruneDeadVehicles() -> typename decltype(vehicles)::size_type
{
    auto initialSize = vehicles.size();

    vehicles.erase(std::remove_if(vehicles.begin(), vehicles.end(),
                                  [this](Vehicle const& v) {
                                      if (v.getHealth() <= 0) {
                                          deadCounter++;
                                          return true;
                                      }
                                      return false;
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
    ss << "(World: [" << width << "x" << height << "] seed: " << seed << ") "
       << "Vehicles: " << vehicles.size() << " | Food: " << food.size()
       << " | Dead Vehicles: " << deadCounter
       << " | Born Vehicles: " << bornCounter << " | Oldest Vehicle: " << maxAge
       << " | Time Elapsed: "
       << std::chrono::duration_cast<std::chrono::seconds>(
              std::chrono::steady_clock::now() - startTime)
              .count()
       << "s"
       << " | Tick: " << tickCounter << " | TPS: " << tps();
    return ss;
}

bool World::tick()
{
    static std::vector<Vehicle> offspring;

    if (rand() % 100 < FOOD_PCT_CHANCE && food.size() < MAX_FOOD) {
        newFood();
    }

    for (auto& food : food) {
        food.update();
    }

    for (auto& vehicle : vehicles) {
        vehicle.align(vehicles);
        vehicle.cohere(vehicles);
        vehicle.avoid(vehicles);
        vehicle.eat(food);
        auto child = vehicle.reproduce(vehicles);
        if (child) {
            offspring.push_back(*child);
        }
        vehicle.attemptMalice(vehicles);
        vehicle.attemptAltruism(vehicles);
        vehicle.update();
        vehicle.avoidEdges();
    }

    // Remove dead vehicles
    pruneDeadVehicles();
    // Remove eaten food
    pruneEatenFood();

    // Add offspring to the
    addAllVehicles(std::move(offspring));
    static_assert(std::is_trivially_destructible<Vehicle>::value,
                  "Vehicle must be trivially destructible for clear()");
    offspring.clear();
    // usleep(10000);

    tickCounter++;
    return vehicles.size() > 0;
}

Vehicle& World::createVehicle(Vec2D const& position)
{
    auto& v = vehicles.emplace_back(position);
    v.world = this;
    return v;
}
