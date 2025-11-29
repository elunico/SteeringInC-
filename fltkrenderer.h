#ifndef RENDERER_H
#define RENDERER_H

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>
#include <FL/x.H>
#include "food.h"
#include "irenderer.h"
#include "vehicle.h"
#include "world.h"

struct FLTKCustomDrawer : public Fl_Box {
    World* world;
    FLTKCustomDrawer(World* world, int W, int H);

    void draw() override;

    void drawVehicle(Vehicle const& vehicle);

    void drawFood(Food const& position);

    void drawLivingWorld();

    void drawDeadWorld();

    void clearScreen();
};

struct FLTKRenderer : public IRenderer {
    World*            world;
    FLTKCustomDrawer* drawer;
    Fl_Window*        window;

    FLTKRenderer(World* world, int W, int H);

    void clearScreen() override;

    void render() override;

    ~FLTKRenderer();
};

#endif  // RENDERER_H