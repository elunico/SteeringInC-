#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <unistd.h>
#include <cassert>
#include <fstream>
#include <ios>
#include <iostream>
#include <istream>
#include <vector>
#include "fltkrenderer.h"
#include "utils.h"

struct arguments {
    bool useCurses        = false;
    bool autoStart        = true;
    int  width            = 800;
    int  height           = 600;
    int  startingVehicles = 10;
};

void parse_args(int argc, char* argv[], arguments& args)
{
    int c;
    while ((c = getopt(argc, argv, "w:h:s:cp")) != -1) {
        switch (c) {
            case 'p':
                args.autoStart = false;
                break;
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
                             "[-c (use curses)] [-p (pause)]\n";
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
    if (args.autoStart) {
        World::isPaused = false;
    } else {
        World::isPaused = true;
    }

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

    output("Simulation ended.\n");

    // report vehicle fitness at the end of simulation
    int count = 1;
    for (auto [id, vehicle] : world.vehicles) {
        output("Vehicle #", count++, " ID: ", id, " age: ", vehicle.get_age(),
               " health: ", vehicle.get_health(),
               " fitness: ", vehicle.get_fitness(), "\n");
    }

    output(world.infoStream().str(), "\n");

    return 0;
}