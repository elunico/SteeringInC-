//
// Created by Thomas Povinelli on 11/29/25.
//

#include "cursesrenderer.h"
#include <ncurses.h>

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

CursesRenderer::~CursesRenderer()
{
    endwin();
}

void CursesRenderer::clearScreen()
{
    clear();
}

void CursesRenderer::drawFood(Food const& food)
{
    mvaddch(food.position.y, food.position.x, ACS_DIAMOND);
}

void CursesRenderer::drawVehicle(Vehicle const& vehicle)
{
    mvaddch(vehicle.getPosition().y, vehicle.getPosition().x, ACS_CKBOARD);
}

void CursesRenderer::drawLivingWorld()
{
    for (auto& food : world->food) {
        drawFood(food);
    }

    for (auto& vehicle : world->vehicles) {
        drawVehicle(vehicle);
    }
}

void CursesRenderer::drawDeadWorld()
{
    mvaddstr(height / 2, width / 2, "All vehicles have perished.");
}

void CursesRenderer::render()
{
    clearScreen();

    if (!world->vehicles.empty()) {
        drawLivingWorld();
    } else {
        drawDeadWorld();
    }

    refresh();
}