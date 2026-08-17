#include "include/consolerenderer.h"
#include <csignal>
#include <iostream>
#include "checks.h"
#include "include/world.h"
#include "utils.h"

static bool check_poll = false;

void tom::render::ConsoleRenderer::enter_alt_buff() const
{
    console_out("\033[?1049h");
}

void tom::render::ConsoleRenderer::exit_alt_buff() const
{
    console_out("\033[?1049l");
}

tom::render::ConsoleRenderer::ConsoleRenderer(World* world) : world(world)
{
    signal(SIGINT, [](int) { check_poll = true; });
    // print the ansi code to hide the cursor
    console_out("\033[?25l");
}

tom::render::ConsoleRenderer::~ConsoleRenderer()
{
    // print the ansi code to show the cursor
    console_out("\033[?25h");
}

void tom::render::ConsoleRenderer::clear_screen()
{
    console_out("\033[2J\033[1;1H");
}

void tom::render::ConsoleRenderer::interrupt_ask()
{
    enter_alt_buff();
reask:
    // show cursor for options
    tom::clear_screen();
    console_out("\033[?25h");
    console_out(world->info_stream().str());
    console_out("\n\nAvailable Commands:");
    console_out("\np: pause/unpause");
    console_out("\nu: sprint!");
    console_out("\nq: end the simulation");
    console_out("\nm: change the max food allowed");
    console_out("\nf: change the food spawn chance");
    console_out("\nv: add a number of vehicles");
    console_out("\na: add an amount of new food");
    console_out("\ns: return to the simulation");
    console_out("\n\nEnter a command: ");
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
        case 'u':
            World::unlimited_tps = !World::unlimited_tps;
            check_poll           = false;
            break;
        case 'm': {
            tom::output("Enter the new max food count: ");
            int max;
            std::cin >> max;
            GUARD(max >= 0);
            GUARD(max <= 10000);
            world->max_food = max;
        } break;
        case 'f': {
            tom::output("Enter the food spawn chance (0-100%): ");
            float chance;
            std::cin >> chance;
            GUARD(chance >= 0 && chance <= 100);
            world->food_pct_chance = chance;
        } break;
        case 'v': {
            tom::output("Enter number of new vehicles to add: ");
            int count;
            std::cin >> count;
            GUARD(count >= 0);
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
            GUARD(count >= 0);
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
    else
        exit_alt_buff();
}

void tom::render::ConsoleRenderer::render(bool transient)
{
    // only update the console every 10 ticks to reduce flicker
    if (transient) {
        console_out(tom::ansi::red);
        return;
    }
    auto message = world->info_stream().str();
    console_out(message);
    std::string backspaces;
    if (message.size() < pmessage_size) {
        auto blanks = std::string(pmessage_size - message.size(), ' ');
        console_out(blanks);
        backspaces = std::string(message.size() + blanks.size(), '\b');
    } else {
        backspaces = std::string(message.size(), '\b');
    }
    console_out(backspaces);

    pmessage_size = message.size();
    console_out(tom::ansi::reset);
    if (check_poll) {
        interrupt_ask();
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
