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
    FLTKCustomDrawer(int W, int H);

    void draw() override;

    void drawVehicle(Vehicle const& vehicle);

    void drawFood(Food const& position);

    void drawLivingWorld();

    void drawDeadWorld();

    void drawQuadtree(QuadTree<Vehicle, Rectangle> const& quad_tree);

    void clearScreen() const;

    int handle(int) override;
};

struct FLTKRenderer : public IRenderer {
    FLTKCustomDrawer* drawer;
    Fl_Window*        window;

    FLTKRenderer(int W, int H);

    void clearScreen() override;

    void render(World*) override;

    void refresh() override;

    ~FLTKRenderer() override;
};

#endif  // RENDERER_H