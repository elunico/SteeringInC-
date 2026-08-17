#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#ifdef NOGUI
#include "consolerenderer.h"
#else
#include <FL/Fl.H>
#include "ui/fltkrenderer.h"
#endif
#include "food.h"
#include "irenderer.h"
#include "utils.h"
#include "vehicle.h"
#include "windows_shim.h"
#include "world.h"

struct arguments {
    double food_pct_chance   = 45.0;
    double edge_threshold    = 25.0;
    int    width             = 800;
    int    height            = 600;
    int    starting_vehicles = 10;
    int    max_food          = 1000;
    int    random_seed       = static_cast<int>(time(nullptr));
    int    start_food        = 200;
    bool   auto_start        = true;
    float  scale_factor      = 1.0f;
    bool   unlimited_tps     = false;
    bool   doNightTime       = true;
};

void lowercase(std::string& s)
{
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
}

arguments parse_args(int argc, char const* argv[])
{
    arguments args;
    int       c;
    while ((c = getopt_shim(argc, argv, "uz:w:h:s:c:pr:e:f:x:n")) != -1) {
        switch (c) {
            case 'n':
                args.doNightTime = false;
                break;
            case 'u':
                args.unlimited_tps = true;
                break;
            case 'f':
                args.start_food = std::stod(optarg_shim);
                break;
            case 'z':
                args.scale_factor = std::stof(optarg_shim);
                break;
            case 'e':
                tom::World::edge_threshold = std::stod(optarg_shim);
                break;
            case 'c':
                args.food_pct_chance = std::stod(optarg_shim);
                break;
            case 'x':
                args.max_food = std::stoul(optarg_shim);
                break;
            case 'p':
                args.auto_start = false;
                break;
            case 'w':
                args.width = std::stoi(optarg_shim);
                break;
            case 'h':
                args.height = std::stoi(optarg_shim);
                break;
            case 's':
                args.starting_vehicles = std::stoi(optarg_shim);
                break;
            case 'r':
                args.random_seed =
                    static_cast<unsigned int>(std::stoul(optarg_shim));
                break;
            default:
                std::cerr << "Unknown option: "
                          << static_cast<char>(optopt_shim) << "\n";
                [[fallthrough]];
            case 'q':
                std::cerr << "Usage: " << argv[0]
                          << " [-w width] [-h height] [-s starting_vehicles] "
                             "[-p (pause)] [-r random_seed] [-e "
                             "edge_threshold] [-f starting_food_count ] [ -c "
                             "food_pct_chance ] [-x max_food] [-z scale_factor "
                             "] [-u (unlimited_tps)] [ -n (disable night) ]\n";
                exit(EXIT_FAILURE);
        }
    }
    if (args.scale_factor <= 0.0f || args.scale_factor >= 5.0) {
        std::cerr << "Scale factor must be 0.0 <= scale factor <= 5.0\n";
        exit(EXIT_FAILURE);
    }
    if (args.width <= 0 || args.height <= 0) {
        std::cerr << "Width and height must be positive integers.\n";
        exit(EXIT_FAILURE);
    }
    if (args.starting_vehicles <= 0) {
        std::cerr << "Starting vehicles must be a positive integer.\n";
        exit(EXIT_FAILURE);
    }
    if (args.edge_threshold <= 0) {
        std::cerr << "Edge threshold must be a positive integer.\n";
        exit(EXIT_FAILURE);
    }
    if (args.start_food <= 0) {
        std::cerr << "Starting food count must be a positive integer.\n";
        exit(EXIT_FAILURE);
    }
    if (args.max_food <= 0) {
        std::cerr << "Max food must be a positive integer.\n";
        exit(EXIT_FAILURE);
    }
    if (args.food_pct_chance < 0.0 || args.food_pct_chance > 100.0) {
        std::cerr << "Food spawn chance must be between 0 and 100.\n";
        exit(EXIT_FAILURE);
    }
    return args;
}

tom::World initialize_world(arguments const& args)
{
    tom::World::is_paused      = !(args.auto_start);
    tom::World::edge_threshold = args.edge_threshold;
    tom::World::unlimited_tps  = args.unlimited_tps;
    tom::World world(args.random_seed, args.width, args.height);
    world.disable_night   = !args.doNightTime;
    world.max_food        = args.max_food;
    world.food_pct_chance = args.food_pct_chance;
    world.populate_world(args.starting_vehicles, args.start_food);
    return world;
}

int main(int argc, char const* argv[])
{
    tom::ansi::cyan.output("./main.cpp use -q for usage information\n");
    arguments args = parse_args(argc, argv);

    tom::set_seed(args.random_seed);

    tom::World world = initialize_world(args);

#ifdef NOGUI
    tom::render::ConsoleRenderer renderer(&world);
#else
    tom::render::FLTKRenderer renderer(&world, args.width, args.height,
                                       args.scale_factor);
#endif

    world.run(renderer);
    tom::output("\nSimulation ended.\n", world.info_stream("\n").str(), "\n");

    return 0;
}
