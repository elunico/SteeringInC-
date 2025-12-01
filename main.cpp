#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <unistd.h>
#include <iostream>
#include <vector>
#include "fltkrenderer.h"

struct arguments {
    bool useCurses        = false;
    int  width            = 800;
    int  height           = 600;
    int  startingVehicles = 10;
};

void parse_args(int argc, char* argv[], arguments& args)
{
    int c;
    while ((c = getopt(argc, argv, "w:h:s:c")) != -1) {
        switch (c) {
            case 'w':
                args.width = std::stoi(optarg);
                break;
            case 'h':
                args.height = std::stoi(optarg);
                break;
            case 'c':
                args.useCurses = true;
                break;
            case 's':
                args.startingVehicles = std::stoi(optarg);
                break;
            default:
                std::cerr << "Unknown option: " << static_cast<char>(optopt)
                          << "\n";
                std::cerr << "Usage: " << argv[0]
                          << " [-w width] [-h height] [-s startingVehicles] "
                             "[-c (use curses)]\n";
                exit(EXIT_FAILURE);
        }
    }
    if (args.width <= 0 || args.height <= 0) {
        std::cerr << "Width and height must be positive integers.\n";
        exit(EXIT_FAILURE);
    }
    if (args.startingVehicles <= 0) {
        std::cerr << "Starting vehicles must be a positive integer.\n";
        exit(EXIT_FAILURE);
    }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    arguments args;
    parse_args(argc, argv, args);

    const int  width  = args.width;
    const int  height = args.height;
    auto const seed   = 1764448905;  // static_cast<unsigned>(time(nullptr));
    // srand(seed);  // Seed for random number generation
    srand(seed);  // Seed for random number generation

    World world(seed, width, height);
    auto  renderer = new FLTKRenderer(&world, width, height);

    world.setup(args.startingVehicles,
                250);  // startingVehicles vehicles, 250 food items

    world.run(renderer);

    renderer->render();
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