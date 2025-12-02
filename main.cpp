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
    bool         use_curses        = false;
    bool         auto_start        = true;
    int          width             = 800;
    int          height            = 600;
    int          starting_vehicles = 10;
    unsigned int random_seed       = 1764448905;
};

void parse_args(int argc, char* argv[], arguments& args)
{
    int c;
    while ((c = getopt(argc, argv, "w:h:s:cpr:")) != -1) {
        switch (c) {
            case 'p':
                args.auto_start = false;
                break;
            case 'w':
                args.width = std::stoi(optarg);
                break;
            case 'h':
                args.height = std::stoi(optarg);
                break;
            case 'c':
                args.use_curses = true;
                break;
            case 's':
                args.starting_vehicles = std::stoi(optarg);
                break;
            case 'r':
                args.random_seed =
                    static_cast<unsigned int>(std::stoul(optarg));
                break;
            default:
                std::cerr << "Unknown option: " << static_cast<char>(optopt)
                          << "\n";
                std::cerr
                    << "Usage: " << argv[0]
                    << " [-w width] [-h height] [-s starting_vehicles] "
                       "[-c (use curses)] [-p (pause)] [-r random_seed]\n";
                exit(EXIT_FAILURE);
        }
    }
    if (args.width <= 0 || args.height <= 0) {
        std::cerr << "Width and height must be positive integers.\n";
        exit(EXIT_FAILURE);
    }
    if (args.starting_vehicles <= 0) {
        std::cerr << "Starting vehicles must be a positive integer.\n";
        exit(EXIT_FAILURE);
    }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    arguments args;
    parse_args(argc, argv, args);
    if (args.auto_start) {
        World::is_paused = false;
    } else {
        World::is_paused = true;
    }

    const int  width  = args.width;
    const int  height = args.height;
    auto const seed =
        args.random_seed;  // static_cast<unsigned>(time(nullptr));
    // srand(seed);  // Seed for random number generation
    srand(seed);  // Seed for random number generation

    World world(seed, width, height);
    auto  renderer = new FLTKRenderer(&world, width, height);

    world.setup(args.starting_vehicles,
                250);  // starting_vehicles vehicles, 250 food items

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

    output(world.info_stream().str(), "\n");

    return 0;
}