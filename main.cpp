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
    auto [width, height] = std::pair{1800, 1000};
    auto const seed      = 1764448905;  // static_cast<unsigned>(time(nullptr));
    // srand(seed);  // Seed for random number generation
    srand(seed);  // Seed for random number generation

    auto* renderer = new FLTKRenderer(width, height);
    World world(seed, width, height, renderer);

    world.setup(400, 250);  // 50 vehicles, 50 food items

    world.run();

    renderer->render(&world);
    delete renderer;

    std::cout << "Simulation ended.\n";

    // report vehicle fitness at the end of simulation
    for (size_t i = 0; i < world.vehicles.size(); ++i) {
        std::cout << "Vehicle " << i << " age: " << world.vehicles[i].getAge()
                  << " health: " << world.vehicles[i].getHealth()
                  << " fitness: " << world.vehicles[i].getFitness() << "\n";
    }

    std::cout << world.infoStream().str() << std::endl;

    return 0;
}