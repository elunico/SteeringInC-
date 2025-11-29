#include <unistd.h>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <iostream>
#include <utility>
#include <vector>

#include "cursesrenderer.h"
#include "fltkrenderer.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    auto [width, height] = std::pair{1600, 1000};

    auto const seed = static_cast<unsigned>(time(nullptr));
    srand(seed);  // Seed for random number generation

    World world(seed, width, height);
    auto* renderer = new FLTKRenderer(&world, width, height);

    world.setup(100, 250);  // 50 vehicles, 50 food items

    while (World::gameRunning) {
        if (!world.tick()) {
            // all vehicles are dead
            break;
        }
        renderer->render();
        // usleep(10000);
    }

    renderer->render();
    delete renderer;

    std::cout << "\nSimulation ended after " << world.tickCounter
              << " ticks (Seed " << seed << ").\n";
    // report vehicle fitness at the end of simulation
    for (size_t i = 0; i < world.vehicles.size(); ++i) {
        std::cout << "Vehicle " << i << " age: " << world.vehicles[i].getAge()
                  << " health: " << world.vehicles[i].getHealth()
                  << " fitness: " << world.vehicles[i].getFitness() << "\n";
    }

    return 0;
}