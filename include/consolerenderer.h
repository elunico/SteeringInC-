#ifndef CONSOLERENDERER_H
#define CONSOLERENDERER_H

#include <include/irenderer.h>
#include <iostream>

namespace tom::render {
struct ConsoleRenderer : public IRenderer {
    World* world;
    ConsoleRenderer(World* world);

    virtual void clear_screen() override;

    virtual void render(bool transient) override;

    virtual void render() override;

    virtual void refresh() override;

    virtual void terminate() override;

    virtual ~ConsoleRenderer();
};

template <typename... Args>
void console_out(Args&&... args)
{
    ((std::cout << std::forward<Args>(args)), ...);
}

}  // namespace tom::render

#endif  // CONSOLERENDERER_H