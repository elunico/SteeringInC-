//
// Created by Thomas Povinelli on 11/29/25.
//

#include "cursesrenderer.h"
#include <ncurses.h>
#include "world.h"

namespace tom::render {
CursesRenderer::CursesRenderer(World* world, int width, int height)
    : world(world), width(width), height(height)
{
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    refresh();
}

void CursesRenderer::clear_screen()
{
    clear();
}

void CursesRenderer::draw_food(Food const& food)
{
    mvaddch(food.position.y, food.position.x, ACS_DIAMOND);
}

void CursesRenderer::draw_vehicle(Vehicle const& vehicle)
{
    mvaddch(vehicle.get_position().y, vehicle.get_position().x, ACS_CKBOARD);
}

void CursesRenderer::draw_living_world(World* world)
{
    auto ss = world->info_stream();
    mvaddstr(0, 0, ss.str().c_str());

    for (auto& [id, food] : world->food) {
        draw_food(food);
    }

    for (auto& [id, vehicle] : world->vehicles) {
        draw_vehicle(vehicle);
    }
}

void CursesRenderer::draw_dead_world()
{
    mvaddstr(height / 2, width / 2, "All vehicles have perished.");
}

void CursesRenderer::render([[maybe_unused]] bool transient)
{
    clear_screen();

    if (!world->vehicles.empty()) {
        draw_living_world(world);
    } else {
        draw_dead_world();
    }

    refresh();
}

void CursesRenderer::terminate()
{
    endwin();
}

void CursesRenderer::refresh()
{
    ::refresh();
}
CursesRenderer::~CursesRenderer()
{
    // endwin();
}

}  // namespace tom::render