#include "include/consolerenderer.h"
#include <iostream>
#include "include/world.h"
#include "utils.h"

static bool check_poll = false;

tom::render::ConsoleRenderer::ConsoleRenderer(World* world) : world(world)
{
    signal(SIGALRM, [](int) { check_poll = true; });
}

tom::render::ConsoleRenderer::~ConsoleRenderer() = default;

void tom::render::ConsoleRenderer::clear_screen()
{
    console_out("\033[2J\033[1;1H");
}

void tom::render::ConsoleRenderer::render(bool transient)
{
    if (transient) {
        console_out("\033[1;1H");
    }
    auto message = world->info_stream().str();
    console_out(message);
    auto backspaces = std::string(message.size(), '\b');
    console_out(backspaces);

    // poll stdin for a keypress to take some action effecting the simulation
    // ...

reask:
    if (check_poll) {
        tom::clear_screen();
        tom::output(world->info_stream().str());
        tom::output("\n\nAvailable Commands:");
        tom::output("\np: pause/unpause");
        tom::output("\nq: end the simulation");
        tom::output("\nm: change the max food allowed");
        tom::output("\nf: change the food spawn chance");
        tom::output("\nv: add a number of vehicles");
        tom::output("\na: add an amount of new food");
        tom::output("\ns: return to the simulation");
        tom::output("\n\nEnter a command: ");
        char c;
        std::cin >> c;

        switch (c) {
            case 'p':
                World::is_paused = !World::is_paused;
                break;
            case 'q':
                World::was_interrupted = true;
                check_poll             = false;
                break;
            case 'm': {
                tom::output("Enter the new max food count: ");
                int max;
                std::cin >> max;
                world->max_food = max;
            } break;
            case 'f': {
                tom::output("Enter the food spawn chance (0-100%): ");
                float chance;
                std::cin >> chance;
                world->food_pct_chance = chance;
            } break;
            case 'v': {
                tom::output("Enter number of new vehicles to add: ");
                int count;
                std::cin >> count;
                world->delay([this, count](auto*) {
                    for (int i = 0; i < count; i++) {
                        world->create_vehicle(world->rand_pos_in_bounds());
                    }
                });
            } break;
            case 'a': {
                tom::output("Enter amount of new food to add: ");
                int count;
                std::cin >> count;
                world->delay([this, count](auto) {
                    for (int i = 0; i < count; i++) {
                        world->new_random_food();
                    }
                });
            } break;
            case 's':
                check_poll = false;
                break;
        }
        tom::clear_screen();
        if (check_poll)
            goto reask;
    }
}

void tom::render::ConsoleRenderer::render()
{
    render(false);
}

void tom::render::ConsoleRenderer::refresh()
{
    console_out("\033[1;1H");
}

void tom::render::ConsoleRenderer::terminate()
{
    render(true);
}
