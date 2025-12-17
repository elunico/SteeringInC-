//
// Created by Thomas Povinelli on 11/29/25.
//

#ifndef CURSESRENDERER_H
#define CURSESRENDERER_H
#include "irenderer.h"
#include "world.h"

namespace tom::render {

class CursesRenderer : public IRenderer {
    World* world;
    int    width;
    int    height;

   public:
    CursesRenderer(World* world, int width, int height);

    void clear_screen() override;
    void draw_food(Food const& food);
    void draw_vehicle(Vehicle const& vehicle);
    void draw_living_world(World* world);
    void draw_dead_world();
    void render(bool transient) override;

    void terminate() override;

    void refresh() override;

    ~CursesRenderer();
};

#endif  // CURSESRENDERER_H
}