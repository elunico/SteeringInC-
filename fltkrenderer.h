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

    void draw_vehicle(Vehicle const& vehicle);

    void draw_food(Food const& position);

    void draw_living_world();

    void draw_dead_world();

    void clear_screen() const;

    int handle(int) override;

    ~FLTKCustomDrawer() override;
};

struct FLTKRenderer : public IRenderer {
    World*            world;
    bool              has_control_window = false;
    FLTKCustomDrawer* drawer;
    static Fl_Window* window;
    static Fl_Window* control_window;

    FLTKRenderer(World* world, int W, int H);

    void clear_screen() override;

    void render() override;

    void refresh() override;

    ~FLTKRenderer() override;

    static void teardown();
};

#endif  // RENDERER_H