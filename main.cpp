#include <ncurses.h>
#include <sys/signal.h>
#include <unistd.h>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <vector>
#include "vec2d.h"
#include "vehicle.h"

#include "utils.h"

int main()
{
    initscr();

    cbreak();
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    printw("Vehicle Simulation Started. Press Ctrl+C to stop.\n");

    auto seed = static_cast<unsigned>(time(0));
    srand(seed);  // Seed for random number generation

    World world(200, 60);
    world.newFood();

    for (int i = 0; i < 20; ++i) {
        world.createVehicle(
            Vec2D(randomInRange(0, 100), randomInRange(0, 100)));
    }

    std::vector<Vehicle> offspring;
    while (World::gameRunning) {
        clear();
        mvprintw(0, 0, "Seed %u: Population %d; Food Count: %d", seed,
                 world.vehicles.size(), world.food.size());

        // if (getch() == 'q') {
        //     World::stopRunning(0);
        //     break;
        // }

        if (world.vehicles.empty()) {
            mvprintw(2, 0, "All vehicles have died. Simulation ended.");
            mvprintw(3, 0, "Press any key to exit.");
            refresh();
            nodelay(stdscr, FALSE);
            getch();
            break;
        }

        if (rand() % 100 < 10 && world.food.size() < 50) {
            world.newFood();
        }

        for (auto& food : world.food) {
            food.update();
            food.show();
        }

        for (auto& vehicle : world.vehicles) {
            vehicle.update();
            vehicle.eat(world.food);
            vehicle.cohere(world.vehicles);
            vehicle.avoid(world.vehicles);
            auto child = vehicle.reproduce(world.vehicles);
            if (child) {
                offspring.push_back(*child);
            }

            // std::cout << "Population size : " << world.vehicles.size() << "
            // ";
            vehicle.show();
        }

        refresh();
        // Remove dead vehicles
        world.pruneDeadVehicles();
        // Remove eaten food
        world.pruneEatenFood();

        // Add offspring to the world
        for (auto& child : offspring) {
            world.addVehicle(std::move(child));
        }
        offspring.clear();

        usleep(10000);
    }
    endwin();
    std::cout << "\nSimulation ended (Seed " << seed << ").\n";
    // report vehicle fitness at end of simulation
    for (size_t i = 0; i < world.vehicles.size(); ++i) {
        std::cout << "Vehicle " << i << " age: " << world.vehicles[i].getAge()
                  << " health: " << world.vehicles[i].getHealth()
                  << " fitness: " << world.vehicles[i].getFitness() << "\n";
    }

    return 0;
}