#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Window.H>
#include <sys/signal.h>
#include <unistd.h>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <utility>
#include <vector>
#include "vec2d.h"
#include "vehicle.h"

#include "renderer.h"
#include "utils.h"

int main(int argc, char* argv[])
{
    auto [width, height] = std::pair{800, 600};

    Fl_Window* window = new Fl_Window(width, height);
    auto       seed   = static_cast<unsigned>(time(0));
    srand(seed);  // Seed for random number generation

    World     world(seed, width, height);
    Renderer* renderer = new Renderer(&world, 0, 0, width, height);
    window->end();
    window->show(argc, argv);
    for (int i = 0; i < 50; ++i) {
        world.newFood();
    }

    for (int i = 0; i < 20; ++i) {
        world.createVehicle(world.randomPosition());
    }

    while (World::gameRunning) {
        if (!world.tick()) {
            // all vehicles are dead
            break;
        }
        renderer->redraw();
        Fl::check();
    }

    renderer->redraw();
    while (Fl::wait())
        ;
    std::cout << "\nSimulation ended after " << world.tickCounter
              << " ticks (Seed " << seed << ").\n";
    // report vehicle fitness at end of simulation
    for (size_t i = 0; i < world.vehicles.size(); ++i) {
        std::cout << "Vehicle " << i << " age: " << world.vehicles[i].getAge()
                  << " health: " << world.vehicles[i].getHealth()
                  << " fitness: " << world.vehicles[i].getFitness() << "\n";
    }

    return 0;
}