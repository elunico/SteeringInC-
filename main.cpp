#include <FL/Fl.H>
#include <unistd.h>
#include <algorithm>
#include <iostream>
#include <ranges>
#include "cursesrenderer.h"
#include "fltkrenderer.h"
#include "irenderer.h"
#include "struct2this.h"
#include "utils.h"
#include "vehicle.h"

struct arguments {
    double       food_pct_chance   = 45.0;
    double       edge_threshold    = 25.0;
    int          width             = 800;
    int          height            = 600;
    int          starting_vehicles = 10;
    unsigned int max_food          = 1000;
    unsigned int random_seed       = static_cast<unsigned int>(time(nullptr));
    bool         use_curses        = false;
    bool         auto_start        = true;
};

void parse_args(int argc, char* argv[], arguments& args)
{
    int c;
    while ((c = getopt(argc, argv, "w:h:s:cpr:e:f:x:")) != -1) {
        switch (c) {
            case 'e':
                tom::World::edge_threshold = std::stod(optarg);
                break;
            case 'f':
                args.food_pct_chance = std::stod(optarg);
                break;
            case 'x':
                args.max_food = std::stoul(optarg);
                break;
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

tom::World initialize_world(arguments const&   args,
                            unsigned int const seed,
                            int const          width,
                            int const          height)
{
    tom::set_seed(seed);
    tom::World::is_paused      = !(args.auto_start);
    tom::World::edge_threshold = args.edge_threshold;
    tom::World world(seed, width, height);
    world.max_food        = args.max_food;
    world.food_pct_chance = args.food_pct_chance;
    world.populate_world(args.starting_vehicles, 250);
    return world;
}

int main(int argc, char* argv[])
{
    arguments args;
    parse_args(argc, argv, args);
    unsigned int const seed = args.random_seed;

    int const width  = args.width;
    int const height = args.height;

    tom::World world = initialize_world(args, seed, width, height);

    auto* const renderer =
        args.use_curses
            ? static_cast<tom::render::IRenderer*>(
                  new tom::render::CursesRenderer(&world, width, height))
            : static_cast<tom::render::IRenderer*>(
                  new tom::render::FLTKRenderer(&world, width, height));

    world.run(renderer);
    renderer->render(tom::World::was_interrupted);
    delete renderer;

    tom::output("Simulation ended.\n");

    if (!world.vehicles.empty()) {
        auto max_fitness_vehicle =
            std::ranges::max_element(world.vehicles | std::views::values,
                                     This2Param{&tom::Vehicle::is_less_fit})
                .base()
                ->second;

        tom::output("Max fitness vehicle ID: ", max_fitness_vehicle.id,
                    " fitness: ", max_fitness_vehicle.get_fitness(), "\n");
    }

    std::string s = world.info_stream().str();
    std::ranges::transform(s, std::begin(s),
                           [](auto const& c) { return c == '|' ? '\n' : c; });
    tom::output(s);

    return 0;
}
