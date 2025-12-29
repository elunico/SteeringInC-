#ifndef RENDERER_H
#define RENDERER_H

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#include "food.h"
#include "irenderer.h"
#include "world.h"

namespace tom::render {
struct FLTKCustomDrawer : public Fl_Box {
    World* world{};
    FLTKCustomDrawer(World* world, int W, int H);
    FLTKCustomDrawer(FLTKCustomDrawer const&)            = delete;
    FLTKCustomDrawer& operator=(FLTKCustomDrawer const&) = delete;

    void draw() override;

    void draw_vehicle(Vehicle const& vehicle);

    void draw_food(Food const& food_item);

    void draw_living_world();

    void draw_dead_world();

    void clear_screen() const;

    int handle(int) override;

    void draw_vehicle_target(Fl_Color     color,
                             Vec2D const& start,
                             Vec2D const& pos);

    ~FLTKCustomDrawer() override;
};

struct FLTKRenderer : public IRenderer {
    World*            world;
    FLTKCustomDrawer* drawer;
    bool              has_control_window = false;
    static Fl_Window* window;
    static Fl_Window* control_window;

    FLTKRenderer(World* world, int W, int H);

    void clear_screen() override;

    void render(bool transient) override;

    void refresh() override;

    void terminate() override;

    ~FLTKRenderer() override;

    static void teardown();
};

}  // namespace tom::render

#endif  // RENDERER_H
