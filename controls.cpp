#include "controls.h"

#include <FL/fl_ask.H>
#include <utility>
#include "fltkrenderer.h"
#include "world.h"

int QtButtonBase::x = 10;
int QtButtonBase::y = 10;

QtToggleButton::QtToggleButton(World*                  world,
                               int                     w,
                               char const*             label,
                               Fl_Color                text_color,
                               Fl_Color                on_color,
                               std::function<int(int)> callback,
                               std::function<bool()>   is_on)
    : QtButtonBase(world, std::move(callback)),
      Fl_Button(QtButtonBase::x, QtButtonBase::y, w, QtButtonBase::h, label),
      on_color(on_color),
      text_color(text_color),
      is_on(std::move(is_on))
{
    box(FL_UP_BOX);
    color(FL_GRAY);
    align(FL_ALIGN_INSIDE | FL_ALIGN_CENTER);

    QtButtonBase::y += QtButtonBase::h + 10;
}

void QtToggleButton::draw()
{
    if (is_on()) {
        fl_color(on_color);
    } else {
        fl_color(FL_GRAY);
    }
    fl_rectf(Fl_Button::x(), Fl_Button::y(), Fl_Button::w(), Fl_Button::h());

    fl_color(text_color);
    fl_font(FL_HELVETICA_BOLD, 14);
    int text_width = fl_width(label()) + 1;
    fl_draw(label(), Fl_Button::x() + (Fl_Button::w() - text_width) / 2,
            Fl_Button::y() + (Fl_Button::h() + fl_height()) / 2 - 4);
}

int QtToggleButton::handle(int event)
{
    if (event == FL_PUSH) {
        if (QtButtonBase::callback) {
            return QtButtonBase::callback(event);  // Example argument
        }
        return 0;
    }
    return Fl_Button::handle(event);
}

QtCheckButton::QtCheckButton(World*                  world,
                             int                     w,
                             char const*             label,
                             std::function<int(int)> callback)
    : QtButtonBase(world, std::move(callback)),
      Fl_Check_Button(QtButtonBase::x,
                      QtButtonBase::y,
                      w,
                      QtButtonBase::h,
                      label)
{
    box(FL_FLAT_BOX);
    color(FL_GRAY);
    align(FL_ALIGN_INSIDE | FL_ALIGN_CENTER);

    QtButtonBase::y += QtButtonBase::h + 10;
}

int QtCheckButton::handle(int event)
{
    if (event == FL_PUSH) {
        if (QtButtonBase::callback) {
            return QtButtonBase::callback(event);  // Example argument
        }
        return 0;
    }
    return Fl_Check_Button::handle(event);
}

Qt_input::Qt_input(World*                  world,
                   int                     w,
                   char const*             label,
                   std::function<int(int)> callback)
    : QtButtonBase(world, std::move(callback)),
      Fl_Input(QtButtonBase::x, QtButtonBase::y, w, QtButtonBase::h, label)
{
    box(FL_FLAT_BOX);
    color(FL_WHITE);
    align(FL_ALIGN_INSIDE | FL_ALIGN_CENTER);

    QtButtonBase::y += QtButtonBase::h + 10;
}

int Qt_input::handle(int event)
{
    if (event == FL_PUSH) {
        if (QtButtonBase::callback) {
            return QtButtonBase::callback(event);  // Example argument
        }
        return 0;
    }
    return Fl_Input::handle(event);
}

ControlWindow::ControlWindow(World* world, int start_x, int W, int H)
    : Fl_Window(start_x, 0, W, H, "Control Window"), world(world)
{
    box(FL_UP_BOX);
    color(FL_LIGHT2);
    align(FL_ALIGN_INSIDE | FL_ALIGN_TOP | FL_ALIGN_LEFT);

    static int const button_width = W - 20;

    buttons.push_back(std::make_unique<QtToggleButton>(
        world, button_width, "Pause/Step", FL_BLACK, FL_BLUE,
        [world, this](int) {
            if (World::is_paused) {
                world->tick();
            } else {
                World::pause();
            }
            redraw();
            return 1;  // Indicate handled
        },
        [] { return World::is_paused; }));

    buttons.push_back(std::make_unique<QtToggleButton>(
        world, button_width, "Resume Simulation", FL_BLACK, FL_GRAY,
        [this](int) {
            if (World::is_paused)
                World::unpause();
            redraw();
            return 1;  // Indicate handled
        }));

    buttons.push_back(std::make_unique<QtToggleButton>(
        world, button_width, "Clear Vehicle Selection", FL_BLACK, FL_GRAY,
        [world](int) {
            world->clear_verbose_vehicles();

            return 1;  // Indicate handled
        }));

    buttons.push_back(std::make_unique<QtToggleButton>(
        world, button_width, "Toggle Sought Vehicles", FL_BLACK, FL_BLUE,
        [this](int) {
            World::show_sought_vehicles = !World::show_sought_vehicles;
            redraw();
            return 1;  // Indicate handled
        },
        [] { return World::show_sought_vehicles; }));

    buttons.push_back(std::make_unique<QtToggleButton>(
        world, button_width, "Add Vehicle", FL_BLACK, FL_GRAY, [world](int) {
            world->create_vehicle(world->random_position());
            return 1;  // Indicate handled
        }));

    buttons.push_back(std::make_unique<QtToggleButton>(
        world, button_width, "End Simulation", FL_RED, FL_GRAY, [](int) {
            World::stop_running(0);
            FLTKRenderer::teardown();
            return 1;  // Indicate handled
        }));

    buttons.push_back(std::make_unique<QtToggleButton>(
        world, button_width, "Kill Mode", FL_BLACK, FL_RED,
        [this](int) {
            World::kill_mode = !World::kill_mode;
            if (!World::kill_mode) {
                redraw();
                return 1;  // Indicate handled
            }
            auto s = fl_input("Enter kill radius");
            if (s) {
                try {
                    World::kill_radius = std::stoi(s);
                } catch (std::exception&) {
                    fl_alert("Invalid radius input. Using default 100.");
                    World::kill_radius = 100;
                }
            } else {
                World::kill_mode = false;
            }
            redraw();
            return 1;  // Indicate handled
        },
        [] { return World::kill_mode; }));

    end();
}

int ControlWindow::handle(int event)
{
    return Fl_Window::handle(event);
}

void ControlWindow::draw()
{
    Fl_Window::draw();
}

QtButtonBase::~QtButtonBase() = default;