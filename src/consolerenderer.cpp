#include "include/consolerenderer.h"
#include "include/world.h"
#include "utils.h"

tom::render::ConsoleRenderer::ConsoleRenderer(World* world) : world(world)
{
}

tom::render::ConsoleRenderer::~ConsoleRenderer() = default;

void tom::render::ConsoleRenderer::clear_screen()
{
    console_out("\033[2J\033[1;1H");
}

void tom::render::ConsoleRenderer::render(bool transient)
{
    if (transient) {
        console_out("\033[1;1H");
    }
    auto message = world->info_stream().str();
    console_out(message);
    auto backspaces = std::string(message.size(), '\b');
    console_out(backspaces);
}

void tom::render::ConsoleRenderer::render()
{
    render(false);
}

void tom::render::ConsoleRenderer::refresh()
{
    console_out("\033[1;1H");
}

void tom::render::ConsoleRenderer::terminate()
{
    render(true);
}
