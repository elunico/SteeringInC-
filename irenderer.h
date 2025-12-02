#ifndef IRENDERER_H
#define IRENDERER_H

struct World;

class IRenderer {
   public:
    // IRenderer(World* world, int W, int H) = 0;

    virtual void clear_screen() = 0;

    // virtual void draw_vehicle(Vehicle const& vehicle) = 0;

    // virtual void draw_food(Food const& position) = 0;

    // virtual void draw_living_world() = 0;

    // virtual void draw_dead_world() = 0;

    virtual void render() = 0;

    virtual void refresh() = 0;

    virtual ~IRenderer() = default;
};

#endif  // IRENDERER_H