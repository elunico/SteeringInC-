#include "ui/fltkrenderer.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <ranges>
#include <sstream>
#include "ui/controls.h"
// #include "ui/qtbuttonbase.h"
#include "utils.h"
#include "vehicle.h"
#include "world.h"

namespace tom::render {

FLTKCustomDrawer::FLTKCustomDrawer(World* world, int W, int H)
    : Fl_Box(0, 0, W, H, nullptr), world(world)
{
}

Fl_Window* FLTKRenderer::window         = nullptr;
Fl_Window* FLTKRenderer::control_window = nullptr;
Fl_Window* FLTKRenderer::info_window    = nullptr;
Fl_Box*    FLTKRenderer::info_label     = nullptr;

FLTKRenderer::FLTKRenderer(World* world, int W, int H, float scale_factor)
    : world(world), scale_factor(scale_factor)
{
#if defined(FL_ABI_VERSION) && (FL_ABI_VERSION >= 10400)
    if (Fl::screen_scaling_supported() > 0) {
        for (decltype(Fl::screen_count()) i = Fl::screen_count() - 1; i >= 0;
             --i) {
            Fl::screen_scale(i, scale_factor);
        }
    } else if (scale_factor != 1.0f) {
        tom::output("Scaling is not supported on this platform!\n");
    }
#else
    tom::output("Scaling is not supported by this version of FLTK\n");
#endif

    Fl::set_font(FL_COURIER, "Menlo");

    window = new Fl_Window(W, H);
    window->label("Vehicle Simulation");

    // am i to understand that this Fl_Window owns the drawer bc it is a
    // subclass of Fl_Box? Therefore when the window is destroyed the drawer
    // is deleted since it is an Fl object in the window and therefore my
    // double free is caused by calling delete drawer after the destructor
    // runs for FLTKRenderer?
    drawer = new FLTKCustomDrawer(world, W, H);
    Fl::set_atclose([](auto closing_window, auto) {
        if (window == closing_window) {
            tom::World::stop_running(0);
            FLTKRenderer::teardown();
        } else {
            closing_window->hide();
        }
    });

    window->end();
    window->show();

    info_window = new Fl_Window(0, std::min(H + 65 + 35, Fl::h() - 10), 700,
                                150, "Information");
    info_window->align(FL_ALIGN_LEFT_TOP | FL_ALIGN_INSIDE);
    info_label = new Fl_Box(0, 0, 700, 100, "Testing");
    info_label->labelfont(FL_COURIER);
    info_window->resizable(info_window);
    info_label->align(FL_ALIGN_LEFT_BOTTOM | FL_ALIGN_INSIDE);
    info_window->end();
    info_window->show();

    control_window =
        new ControlWindow(world, W + 10, FLTKRenderer::CONTROL_WINDOW_WIDTH, H);
    control_window->label("Controls");
    control_window->end();
    control_window->show();
}

void FLTKRenderer::clear_screen()
{
    drawer->clear_screen();
}

void FLTKRenderer::render(bool transient)
{
    drawer->redraw();
    if (!transient) {
        refresh();
    }
}
void FLTKRenderer::refresh()
{
    drawer->redraw();
    Fl::check();
}

void FLTKRenderer::terminate()
{
    teardown();
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

std::array display_colors = {FL_BLUE,   FL_CYAN, FL_GREEN,
                             FL_YELLOW, FL_RED,  FL_MAGENTA};

void FLTKCustomDrawer::draw()
{
    assert(world != nullptr &&
           "World pointer is null. Did you forget to set it?");

    clear_screen();
    if (world->is_day()) {
        fl_color(FL_WHITE);
    } else {
        fl_color(FL_GRAY);
    }
    fl_rectf(x(), y(), w(), h());

    draw_living_world();

    auto ss  = world->info_stream("\n");
    auto msg = ss.str();
    if (FLTKRenderer::info_window) {
        FLTKRenderer::info_label->copy_label(msg.c_str());
    }
    if (ControlWindow::show_info) {
        fl_font(FL_COURIER, 14);
        fl_color(FL_BLACK);
        fl_draw(msg.c_str(), 0, 0, w(), h(), FL_ALIGN_TOP_LEFT | FL_ALIGN_WRAP);
    } else if (world && world->vehicles.empty()) {
        draw_dead_world();
    }
}

void FLTKCustomDrawer::draw_vehicle(Vehicle const& vehicle)
{
    fl_color(FL_BLACK);

    if (vehicle.behavior_state.contains(Vehicle::BehaviorState::UNSET))
        fl_color(FL_GRAY0);

    if (vehicle.behavior_state.contains(Vehicle::BehaviorState::WANDERING))
        fl_color(FL_GREEN);

    if (vehicle.behavior_state.contains(Vehicle::BehaviorState::HUNGRY))
        fl_color(FL_BLUE);

    if (vehicle.behavior_state.contains(Vehicle::BehaviorState::OUTGOING))
        fl_color(FL_RED);

    if (vehicle.behavior_state.contains(Vehicle::BehaviorState::DESPERATE))
        fl_color(FL_MAGENTA);

    Vec2D  pos     = vehicle.get_position();
    double heading = vehicle.get_velocity().heading();
    int    size = remap(vehicle.get_health().remaining(), 0.0, 20.0, 4.0, 10.0);

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
    if (vehicle.get_last_sought_vehicle_id() &&
        World::view_mode.contains(World::ViewMode::VEHICLE_SEEKING)) {
        auto& target = vehicle.last_sought_vehicle().get_position();
        draw_vehicle_target(FL_BLUE, pos, target);
    }

    // Draw  a line from the vehicle to its last sought food if it exists
    if (vehicle.last_sought_food_id != 0 &&
        World::view_mode.contains(World::ViewMode::FOOD_SEEKING)) {
        auto& target = vehicle.last_sought_food().get_position();
        draw_vehicle_target(FL_GREEN, pos, target);
    }
}

void FLTKCustomDrawer::draw_food(Food const& food_item)
{
    auto s = remap(food_item.dna.nutrition, 1.0, 50.0, 5.0, 15.0);

    fl_rectf(food_item.position.x, food_item.position.y, s, s,
             food_item.dna.nutrition < 0 ? FL_RED : FL_GREEN);
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
    fl_font(FL_HELVETICA_BOLD, 18);
    int text_width = fl_width(message.c_str()) + 1;
    fl_draw(message.c_str(), (w() - text_width) / 2, h() / 2);
}

void FLTKCustomDrawer::clear_screen() const
{
    fl_color(FL_WHITE);
    fl_rectf(x(), y(), w(), h());
}

int FLTKCustomDrawer::handle(int i)
{
    if (i == FL_PUSH) {
        double x = Fl::event_x();
        double y = Fl::event_y();
        if (World::interact_mode.contains(World::InteractMode::KILL)) {
            for (auto& [id, vehicle] : world->vehicles) {
                if (vehicle.get_position().distance_to(Vec2D{x, y}) <
                    World::kill_radius) {
                    vehicle.kill();
                }
            }
            return 1;
        }
        if (World::interact_mode.contains(World::InteractMode::FEED)) {
            for (int idx = 0; idx < world->feed_count; idx++) {
                world->new_food(Vec2D{x, y} + Vec2D::random(5), 5.0 / 0.05);
            }
            return 1;
        }
        for (auto& vehicle : world->vehicles | std::views::values) {
            if (vehicle.get_position().distance_to(Vec2D{x, y}) < 30) {
                vehicle.verbose = !vehicle.verbose;
                return 1;
            }
        }
        return Fl_Box::handle(i);
    }
    return Fl_Box::handle(i);
}

void FLTKCustomDrawer::draw_vehicle_target(Fl_Color     color,
                                           Vec2D const& start,
                                           Vec2D const& pos)
{
    fl_color(color);
    fl_line(static_cast<int>(start.x), static_cast<int>(start.y),
            static_cast<int>(pos.x), static_cast<int>(pos.y));
    fl_rect(static_cast<int>(pos.x) - 3, static_cast<int>(pos.y) - 3, 6, 6);
}

FLTKCustomDrawer::~FLTKCustomDrawer() = default;

void FLTKRenderer::teardown()
{
    if (control_window) {
        control_window->hide();
    }
    if (info_window) {
        info_window->hide();
    }
    if (window) {
        window->hide();
    }
}
}  // namespace tom::render
