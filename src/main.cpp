#include <FL/Fl.H>
#include "windows_shim.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <ranges>
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
    bool         auto_start        = true;
    float scale_factor = 1.0f;
};

void parse_args(int argc, char const* argv[], arguments& args)
{
    int c;
    while ((c = getopt_shim(argc, argv, "z:w:h:s:cpr:e:f:x:")) != -1) {
        switch (c) {
            case 'z':
                args.scale_factor = std::stof(optarg_shim);
                break;
            case 'e':
                tom::World::edge_threshold = std::stod(optarg_shim);
                break;
            case 'f':
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
                std::cerr << "Unknown option: " << static_cast<char>(optopt_shim)
                          << "\n";
                std::cerr
                    << "Usage: " << argv[0]
                    << " [-w width] [-h height] [-s starting_vehicles] "
                       "[-p (pause)] [-r random_seed] [-e "
                       "edge_threshold] [-f food_pct_chance] [-x max_food] [-z scale_factor ]\n";
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
    tom::set_seed(seed);
    tom::World::is_paused      = !(args.auto_start);
    tom::World::edge_threshold = args.edge_threshold;
    tom::World world(seed, width, height);
    world.max_food        = args.max_food;
    world.food_pct_chance = args.food_pct_chance;
    world.populate_world(args.starting_vehicles, 250);
    return world;
}

int main(int argc, char const* argv[])
{
    arguments args;
    parse_args(argc, argv, args);

#if defined(FL_ABI_VERSION) && (FL_ABI_VERSION >= 10400)
    if (Fl::screen_scaling_supported() > 0) {
        for (decltype(Fl::screen_count()) i = Fl::screen_count() - 1; i >= 0; --i) {
            Fl::screen_scale(i, args.scale_factor);
        }
    } else if (args.scale_factor != 1.0f) {
        tom::output("Scaling is not supported on this platform!\n");
    }
#else
    tom::output("Scaling is not supported by this version of FLTK\n");
#endif
    
    unsigned int const seed = args.random_seed;

    int const width  = args.width;
    int const height = args.height;

    tom::World world = initialize_world(args, seed, width, height);

    tom::render::FLTKRenderer renderer(&world, width, height);

    world.run(&renderer);
    renderer.render(tom::World::was_interrupted);
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
