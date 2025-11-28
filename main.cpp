#include <sys/signal.h>
#include <unistd.h>
#include <algorithm>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <vector>
#include "vec2d.h"
#include "vehicle.h"

#include "utils.h"

int main()
{
    srand(static_cast<unsigned>(time(0)));  // Seed for random number generation

    World world(100, 100);
    world.newFood();

    for (int i = 0; i < 20; ++i) {
        world.createVehicle(
            Vec2D(randomInRange(0, 100), randomInRange(0, 100)));
    }

    std::vector<Vehicle> offspring;
    while (World::gameRunning) {
        for (auto& vehicle : world.vehicles) {
            vehicle.update();
            vehicle.eat(world.food);
            auto child = vehicle.reproduce(world.vehicles);
            if (child) {
                offspring.push_back(*child);
            }

            std::cout << "Population size : " << world.vehicles.size() << " ";
            vehicle.show();
        }

        // Remove dead vehicles
        world.pruneDeadVehicles();

        // Add offspring to the world
        for (auto& child : offspring) {
            world.addVehicle(std::move(child));
        }
        offspring.clear();

        usleep(100000);
    }
    std::cout << "\nSimulation ended.\n";
    // report vehicle fitness at end of simulation
    for (size_t i = 0; i < world.vehicles.size(); ++i) {
        std::cout << "Vehicle " << i << " age: " << world.vehicles[i].getAge()
                  << " health: " << world.vehicles[i].getHealth()
                  << " fitness: " << world.vehicles[i].getFitness() << "\n";
    }

    return 0;
}