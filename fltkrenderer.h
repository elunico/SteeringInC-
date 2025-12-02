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
    World* world{};
    FLTKCustomDrawer(World* world, int W, int H);

    void draw() override;

    void drawVehicle(Vehicle const& vehicle);

    void drawFood(Food const& position);

    void drawLivingWorld();

    void drawDeadWorld();

    void clearScreen() const;

    int handle(int) override;

    ~FLTKCustomDrawer() override;
};

struct FLTKRenderer : public IRenderer {
    World*            world;
    bool              hasControlWindow = false;
    FLTKCustomDrawer* drawer;
    static Fl_Window* window;
    static Fl_Window* controlWindow;

    FLTKRenderer(World* world, int W, int H);

    void clearScreen() override;

    void render() override;

    void refresh() override;

    ~FLTKRenderer() override;

    static void teardown();
};

#endif  // RENDERER_H