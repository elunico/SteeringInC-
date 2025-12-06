#ifndef IRENDERER_H
#define IRENDERER_H

struct World;

class IRenderer {
   public:
    virtual void clear_screen() = 0;

    virtual void render(bool transient) = 0;

    virtual void render();

    virtual void refresh() = 0;

    virtual void terminate() = 0;

    virtual ~IRenderer() = default;
};

#endif  // IRENDERER_H