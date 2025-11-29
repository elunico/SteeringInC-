#ifndef RENDERER_H
#define RENDERER_H

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>
#include <FL/x.H>
#include "food.h"
#include "vehicle.h"
#include "world.h"

struct Renderer : public Fl_Box {
    World* world;
    Renderer(World* world, int X, int Y, int W, int H, const char* L = 0);

    virtual void clearScreen();

    virtual void draw() override;

    virtual void drawVehicle(Vehicle const& vehicle);

    virtual void drawFood(Food const& position);

    virtual void drawLivingWorld();

    virtual void drawDeadWorld();
};

#endif  // RENDERER_H