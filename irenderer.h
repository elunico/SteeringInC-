#ifndef IRENDERER_H
#define IRENDERER_H

class IRenderer {
   public:
    // IRenderer(World* world, int W, int H) = 0;

    virtual void clearScreen() = 0;

    // virtual void drawVehicle(Vehicle const& vehicle) = 0;

    // virtual void drawFood(Food const& position) = 0;

    // virtual void drawLivingWorld() = 0;

    // virtual void drawDeadWorld() = 0;

    virtual void render() = 0;

    virtual ~IRenderer() = default;
};

#endif  // IRENDERER_H