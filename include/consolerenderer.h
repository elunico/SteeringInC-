#ifndef CONSOLERENDERER_H
#define CONSOLERENDERER_H

#include <include/irenderer.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <csetjmp>
#include <functional>
#include <iostream>
#include "dimensions.h"

namespace tom::render {
struct ConsoleRenderer : public IRenderer {
    World*                      world;
    bool                        has_interrupt_handler;
    std::function<void(World*)> interrupt_handler;
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