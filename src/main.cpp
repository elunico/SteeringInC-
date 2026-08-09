#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <ranges>
#include <string>
#ifdef NOGUI
#include "consolerenderer.h"
#else
#include <FL/Fl.H>
#include "ui/fltkrenderer.h"
#endif
#include "food.h"
#include "irenderer.h"
#include "struct2this.h"
#include "utils.h"
#include "vehicle.h"
#include "windows_shim.h"
#include "world.h"

struct arguments {
    double       food_pct_chance   = 45.0;
    double       edge_threshold    = 25.0;
    int          width             = 800;
    int          height            = 600;
    int          starting_vehicles = 10;
    unsigned int max_food          = 1000;
    unsigned int random_seed       = static_cast<unsigned int>(time(nullptr));
    unsigned int start_food        = 200;
    bool         auto_start        = true;
    float        scale_factor      = 1.0f;
    bool         unlimited_tps     = false;
};

int get_dimension_width(char const* optarg)
{
    return std::stoi(optarg);
}

int get_dimension_height(char const* optarg)
{
    return std::stoi(optarg);
}

void lowercase(std::string& s)
{
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
}

void parse_args(int argc, char const* argv[], arguments& args)
{
    int c;
    while ((c = getopt_shim(argc, argv, "uz:w:h:s:c:pr:e:f:x:")) != -1) {
        switch (c) {
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
                args.width = get_dimension_width(optarg_shim);
                break;
            case 'h':
                args.height = get_dimension_height(optarg_shim);
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
                /* fallthrough */
            case 'q':
                std::cerr << "Usage: " << argv[0]
                          << " [-w width] [-h height] [-s starting_vehicles] "
                             "[-p (pause)] [-r random_seed] [-e "
                             "edge_threshold] [-f starting_food_count ] [ -c "
                             "food_pct_chance ] [-x max_food] [-z scale_factor "
                             "] [-u (unlimited_tps)]\n";
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
}

tom::World initialize_world(arguments const&   args,
                            unsigned int const seed,
                            int const          width,
                            int const          height)
{
    tom::World::is_paused      = !(args.auto_start);
    tom::World::edge_threshold = args.edge_threshold;
    tom::World::unlimited_tps  = args.unlimited_tps;
    tom::World world(seed, width, height);
    world.max_food        = args.max_food;
    world.food_pct_chance = args.food_pct_chance;
    world.populate_world(args.starting_vehicles, args.start_food);
    return world;
}

template <typename T, typename R, typename... Args>
struct GetOwningClass {
    using ClassType = T;

    GetOwningClass(R (T::*_)(Args...))
    {
    }
};

int main(int argc, char const* argv[])
{
    arguments args;
    parse_args(argc, argv, args);

    unsigned int const seed = args.random_seed;
    tom::set_seed(seed);

    int const width  = args.width;
    int const height = args.height;

    tom::World world = initialize_world(args, seed, width, height);

#ifdef NOGUI
    tom::render::ConsoleRenderer renderer(&world);
#else

    tom::render::FLTKRenderer renderer(&world, width, height,
                                       args.scale_factor);
#endif

    world.run(renderer);
    renderer.render(tom::World::was_interrupted);
    tom::output("Simulation ended.\n");

    if (!world.vehicles.empty()) {
        auto max_fitness_vehicle =
            std::ranges::max_element(world.vehicles | std::views::values,
                                     This2Param{&tom::Vehicle::is_less_fit})
                .base()
                ->second;

        tom::output("Max fitness vehicle ID: ", max_fitness_vehicle.id,
                    " fitness: ", max_fitness_vehicle.get_fitness(), "\n\n");
    }

    std::string s = world.info_stream().str();
    std::ranges::transform(s, std::begin(s),
                           [](auto const& c) { return c == '|' ? '\n' : c; });
    tom::output(s);

    return 0;
}
