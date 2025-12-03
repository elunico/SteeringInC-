#include "fltkrenderer.h"
#include <assert.h>
#include <cmath>
#include <sstream>
#include "controls.h"
#include "utils.h"
#include "vehicle.h"
#include "world.h"

Fl_Window* FLTKRenderer::window         = nullptr;
Fl_Window* FLTKRenderer::control_window = nullptr;

QtColor::QtColor(unsigned char r, unsigned char g, unsigned char b)
    : r(r), g(g), b(b)
{
}

QtColor::QtColor(QtColor const& other) : r(other.r), g(other.g), b(other.b)
{
}

QtColor::QtColor() : r(0), g(0), b(0)
{
}

bool QtColor::operator==(QtColor const& other) const
{
    return r == other.r && g == other.g && b == other.b;
}

bool QtColor::operator!=(QtColor const& other) const
{
    return !(*this == other);
}

QtColor& QtColor::operator=(QtColor const& other)
{
    r = other.r;
    g = other.g;
    b = other.b;
    return *this;
}

FLTKRenderer::FLTKRenderer(World* world, int W, int H) : world(world)
{
    window = new Fl_Window(W, H);
    window->label("Vehicle Simulation");

    // am i to understand that this Fl_Window owns the drawer bc it is a
    // subclass of Fl_Box? Therefore when the window is destroyed the drawer is
    // deleted since it is an Fl object in the window and therefore my double
    // free is caused by calling delete drawer after the destructor runs for
    // FLTKRenderer?
    drawer = new FLTKCustomDrawer(world, W, H);
    Fl::set_atclose([](auto, auto) {
        World::stop_running(0);
        FLTKRenderer::teardown();
    });
    window->end();
    window->show();

    control_window = new ControlWindow(world, W + 10, 200, H);
    control_window->label("Controls");
    control_window->end();
    control_window->show();
}

FLTKRenderer::~FLTKRenderer()
{
    while (Fl::wait())
        ;
    // See FLTKRenderer::FLTKRenderer
    // delete drawer;
    delete window;
    delete control_window;
}

void FLTKRenderer::render()
{
    refresh();
}
void FLTKRenderer::refresh()
{
    drawer->redraw();
    Fl::check();
}

void FLTKRenderer::clear_screen()
{
    drawer->clear_screen();
}

FLTKCustomDrawer::FLTKCustomDrawer(World* world, int W, int H)
    : Fl_Box(0, 0, W, H, nullptr), world(world)
{
}

std::array display_colors = {FL_BLUE,   FL_CYAN, FL_GREEN,
                             FL_YELLOW, FL_RED,  FL_MAGENTA};

void FLTKCustomDrawer::clear_screen() const
{
    fl_color(FL_WHITE);
    fl_rectf(x(), y(), w(), h());
}

int FLTKCustomDrawer::handle(int i)
{
    if (i == FL_PUSH) {
        if (World::kill_mode) {
            double x = Fl::event_x();
            double y = Fl::event_y();
            for (auto& [id, vehicle] : world->vehicles) {
                if (vehicle.get_position().distance_to(Vec2D{x, y}) <
                    World::kill_radius) {
                    vehicle.kill();
                }
            }
            return 1;
        }
        double x = Fl::event_x();
        double y = Fl::event_y();
        for (auto& [id, vehicle] : world->vehicles) {
            if (vehicle.get_position().distance_to(Vec2D{x, y}) < 30) {
                vehicle.verbose = !vehicle.verbose;
                return 1;
            }
        }
        return Fl_Box::handle(i);
    }
    return Fl_Box::handle(i);
}

FLTKCustomDrawer::~FLTKCustomDrawer() = default;

void FLTKCustomDrawer::draw_vehicle(Vehicle const& vehicle)
{
    fl_color(FL_BLACK);
    if (vehicle.highlighted) {
        fl_color(FL_RED);
    }
    if (vehicle.verbose) {
        fl_color(FL_BLUE);
    }

    if (vehicle.get_age() < vehicle.get_dna().age_of_maturity) {
        fl_color(FL_MAGENTA);
    }

    Vec2D  pos     = vehicle.get_position();
    double heading = vehicle.get_velocity().heading();
    int    size    = remap(vehicle.get_health(), 0.0, 20.0, 4.0, 10.0);

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

    auto rad = vehicle.get_dna().perception_radius;
    // Draw an empty circle with a thin line to represent perception radius
    if (vehicle.verbose) {
        auto diameter = rad * 2;
        fl_color(FL_GREEN);
        fl_line_style(FL_SOLID, 2);
        fl_arc(static_cast<int>(pos.x - rad), static_cast<int>(pos.y - rad),
               static_cast<int>(diameter), static_cast<int>(diameter), 0, 360);
    }

    // Draw  a line from the vehicle to its last sought vehicle if it exists
    if (vehicle.get_last_sought_vehicle_id() && World::show_sought_vehicles) {
        fl_color(FL_BLUE);
        auto& position = vehicle.last_sought_vehicle().get_position();
        fl_line(static_cast<int>(pos.x), static_cast<int>(pos.y),
                static_cast<int>(position.x), static_cast<int>(position.y));
        fl_rect(static_cast<int>(position.x) - 3,
                static_cast<int>(position.y) - 3, 6, 6);
    }

    // Draw  a line from the vehicle to its last sought food if it exists
    if (vehicle.last_sought_food_id != 0 && World::show_sought_vehicles) {
        fl_color(FL_GREEN);
        auto& position = vehicle.last_sought_food().get_position();
        if (double_near_zero(position.x) && double_near_zero(position.y)) {
            assert(!"That's bad");
        }
        fl_line(static_cast<int>(pos.x), static_cast<int>(pos.y),
                static_cast<int>(position.x), static_cast<int>(position.y));
        fl_rect(static_cast<int>(position.x) - 3,
                static_cast<int>(position.y) - 3, 6, 6);
    }
}

void FLTKCustomDrawer::draw_food(Food const& position)
{
    if (position.nutrition < 0) {
        fl_color(FL_RED);
    } else {
        fl_color(FL_GREEN);
    }
    fl_pie(static_cast<int>(position.position.x) - 2,
           static_cast<int>(position.position.y) - 2, 4, 4, 0, 360);
}

void FLTKCustomDrawer::draw_living_world()
{
    for (auto& [id, food] : world->food) {
        draw_food(food);
    }

    for (auto& [id, vehicle] : world->vehicles) {
        draw_vehicle(vehicle);
    }
}

void FLTKCustomDrawer::draw_dead_world()
{
    fl_color(FL_RED);
    std::string message = "All vehicles have perished.";
    fl_font(FL_HELVETICA_BOLD, 24);
    int text_width = fl_width(message.c_str()) + 1;
    fl_draw(message.c_str(), (w() - text_width) / 2, h() / 2);
}

void FLTKCustomDrawer::draw()
{
    assert(world != nullptr &&
           "World pointer is null. Did you forget to set it?");
    clear_screen();
    auto ss  = world->info_stream();
    auto msg = ss.str();
    fl_font(FL_HELVETICA_BOLD, 14);
    fl_color(FL_BLACK);
    fl_draw(msg.c_str(), 0, 0, w(), h(), FL_ALIGN_TOP_LEFT | FL_ALIGN_WRAP);
    if (!world->vehicles.empty()) {
        draw_living_world();
    } else {
        draw_dead_world();
    }
}

void FLTKRenderer::teardown()
{
    if (control_window) {
        control_window->hide();
    }
    if (window) {
        window->hide();
    }
}
