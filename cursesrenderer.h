//
// Created by Thomas Povinelli on 11/29/25.
//

#ifndef CURSESRENDERER_H
#define CURSESRENDERER_H
#include "food.h"
#include "irenderer.h"
#include "vehicle.h"
#include "world.h"

class CursesRenderer : public IRenderer {
    World* world;
    int    width;
    int    height;

   public:
    CursesRenderer(World* world, int width, int height);

    void clearScreen() override;
    void drawFood(Food const& food);
    void drawVehicle(Vehicle const& vehicle);
    void drawLivingWorld();
    void drawDeadWorld();
    void render() override;

    ~CursesRenderer();
};

#endif  // CURSESRENDERER_H
