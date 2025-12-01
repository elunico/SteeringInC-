#include "fltkrenderer.h"

#include <assert.h>

#include <chrono>
#include <cmath>

#include <iostream>
#include <sstream>
#include "controls.h"
#include "vehicle.h"

Fl_Window* FLTKRenderer::window        = nullptr;
Fl_Window* FLTKRenderer::controlWindow = nullptr;

FLTKRenderer::FLTKRenderer(int W, int H)
{
    window = new Fl_Window(W, H);

    // am i to understand that this Fl_Window owns the drawer bc it is a subclass
    // of Fl_Box? Therefore when the window is destroyed the drawer is deleted
    // since it is an Fl object in the window and therefore my double free
    // is caused by calling delete drawer after the destructor runs for
    // FLTKRenderer?
    drawer = new FLTKCustomDrawer(W, H);
    Fl::set_atclose([](auto, auto) {
        World::stopRunning(0);
        FLTKRenderer::teardown();
    });
    window->end();
    window->show();
}

FLTKRenderer::~FLTKRenderer()
{
    while (Fl::wait())
        ;
    // See FLTKRenderer::FLTKRenderer
    // delete drawer;
    delete window;
    delete controlWindow;
}

void FLTKRenderer::render(World* world)
{
    drawer->world = world;
    if (!hasControlWindow) {
        hasControlWindow = true;
        controlWindow    = new ControlWindow(world, world->width + 10, 200,
                                             (QtButtonBase::h + 15) * 8);
        controlWindow->show();
    }
    refresh();
}
void FLTKRenderer::refresh()
{
    drawer->redraw();
    Fl::check();
}

void FLTKCustomDrawer::drawQuadtree(
    QuadTree<Vehicle, Rectangle> const& quad_tree)
{
    fl_color(FL_RED);

    std::stringstream message;
    message << quad_tree.points.size() << " qty.";
    fl_draw(message.str().c_str(), quad_tree.boundary.topLeft.x,
            quad_tree.boundary.topLeft.y + 15);
    fl_rect(quad_tree.boundary.topLeft.x, quad_tree.boundary.topLeft.y,
            quad_tree.boundary.bottomRight.x - quad_tree.boundary.topLeft.x,
            quad_tree.boundary.bottomRight.y - quad_tree.boundary.topLeft.y,
            FL_RED);
    if (quad_tree.divided) {
        drawQuadtree(*quad_tree.northWest);
        drawQuadtree(*quad_tree.northEast);
        drawQuadtree(*quad_tree.southWest);
        drawQuadtree(*quad_tree.southEast);
    }
}

void FLTKRenderer::clearScreen()
{
    drawer->clearScreen();
}

FLTKCustomDrawer::FLTKCustomDrawer(int W, int H) : Fl_Box(0, 0, W, H, nullptr)
{
}

std::array displayColors = {FL_BLUE,   FL_CYAN, FL_GREEN,
                            FL_YELLOW, FL_RED,  FL_MAGENTA};

void FLTKCustomDrawer::clearScreen() const
{
    fl_color(FL_WHITE);
    fl_rectf(x(), y(), w(), h());
}

int FLTKCustomDrawer::handle(int i)
{
    if (i == FL_PUSH) {
        if (World::killMode) {
            double x = Fl::event_x();
            double y = Fl::event_y();
            for (auto& vehicle : world->vehicles) {
                if (vehicle.getPosition().distanceTo(Vec2D{x, y}) <
                    World::killRadius) {
                    vehicle.kill();
                }
            }
            return 1;
        } else {
            double x = Fl::event_x();
            double y = Fl::event_y();
            for (auto& vehicle : world->vehicles) {
                if (vehicle.getPosition().distanceTo(Vec2D{x, y}) < 10) {
                    vehicle.verbose = !vehicle.verbose;
                    return 1;
                }
            }
            return Fl_Box::handle(i);
        }
    }
    return Fl_Box::handle(i);
}

FLTKCustomDrawer::~FLTKCustomDrawer() = default;

void FLTKCustomDrawer::drawVehicle(Vehicle const& vehicle)
{
    fl_color(FL_BLACK);
    if (vehicle.highlighted) {
        fl_color(FL_RED);
    }
    if (vehicle.verbose) {
        fl_color(FL_BLUE);
    }
    Vec2D  pos     = vehicle.getPosition();
    double heading = vehicle.getVelocity().heading();
    int    size    = remap(vehicle.getHealth(), 0.0, 20.0, 4.0, 10.0);

    // Calculate triangle vertices
    int x1 = static_cast<int>(pos.x + cos(heading) * size);
    int y1 = static_cast<int>(pos.y + sin(heading) * size);
    int x2 = static_cast<int>(pos.x + cos(heading + 2.5) * size);
    int y2 = static_cast<int>(pos.y + sin(heading + 2.5) * size);
    int x3 = static_cast<int>(pos.x + cos(heading - 2.5) * size);
    int y3 = static_cast<int>(pos.y + sin(heading - 2.5) * size);

    // Draw the triangle
    fl_begin_polygon();
    fl_vertex(x1, y1);
    fl_vertex(x2, y2);
    fl_vertex(x3, y3);
    fl_end_polygon();

    // Draw an empty circle with a thin line to represent perception radius
    if (vehicle.verbose) {
        auto rad = vehicle.getDNA().perceptionRadius;
        fl_color(FL_GREEN);
        fl_line_style(FL_SOLID, 2);
        fl_arc(pos.x - rad / 2, pos.y - rad / 2, rad, rad, 0, 360);
    }
}

void FLTKCustomDrawer::drawFood(Food const& position)
{
    fl_color(FL_GREEN);
    fl_pie(static_cast<int>(position.position.x) - 2,
           static_cast<int>(position.position.y) - 2, 4, 4, 0, 360);
}

void FLTKCustomDrawer::drawLivingWorld()
{
    for (auto& food : world->food) {
        drawFood(food);
    }

    for (auto& vehicle : world->vehicles) {
        drawVehicle(vehicle);
    }
}

void FLTKCustomDrawer::drawDeadWorld()
{
    fl_color(FL_RED);
    std::string message = "All vehicles have perished.";
    fl_font(FL_HELVETICA_BOLD, 24);
    int textWidth = fl_width(message.c_str()) + 1;
    fl_draw(message.c_str(), (w() - textWidth) / 2, h() / 2);
}

void FLTKCustomDrawer::draw()
{
    assert(world != nullptr &&
           "World pointer is null. Did you forget to set it?");
    clearScreen();
    auto ss  = world->infoStream();
    auto msg = ss.str();
    fl_font(FL_HELVETICA_BOLD, 14);
    fl_color(FL_BLACK);
    fl_draw(msg.c_str(), 0, 0, w(), h(), FL_ALIGN_TOP_LEFT | FL_ALIGN_WRAP);
    if (!world->vehicles.empty()) {
        drawLivingWorld();
    } else {
        drawDeadWorld();
    }
}

void FLTKRenderer::teardown()
{
    if (controlWindow) {
        controlWindow->hide();
    }
    if (window) {
        window->hide();
    }
}
