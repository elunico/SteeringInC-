#include "controls.h"

#include <FL/fl_ask.H>
#include <string>
#include <utility>
#include "fltkrenderer.h"
#include "utils.h"
#include "world.h"

namespace tom::render {

int QtBase::x = 10;
int QtBase::y = 10;

QtBase::QtBase(int h) : h(h)
{
}

Fl_Color QtButtonBase::default_on_color  = fl_rgb_color(0, 225, 80);

Fl_Color QtButtonBase::default_off_color = fl_rgb_color(200, 200, 200);

Fl_Color QtButtonBase::default_warning_color = fl_rgb_color(255, 30, 30);

QtButtonBase::QtButtonBase(World* world, std::function<int(int)> callback)
    : QtBase(QtButtonBase::button_height),
      world(world),
      callback(std::move(callback))
{
}

QtButtonBase::~QtButtonBase() = default;

QtSeparator::QtSeparator(int w)
    : QtBase(4), Fl_Box(QtBase::x, QtBase::y, w, QtBase::h, "")
{
    box(FL_FLAT_BOX);
    color(FL_DARK3);
    QtBase::y += QtBase::h + 10;
}

void QtSeparator::draw()
{
    fl_color(FL_DARK3);
    fl_rectf(Fl_Box::x(), Fl_Box::y(), Fl_Box::w(), Fl_Box::h());
}

QtToggleButton::QtToggleButton(World*                  world,
                               int                     w,
                               char const*             label,
                               Fl_Color                text_color,
                               Fl_Color                on_color,
                               std::function<int(int)> callback,
                               std::function<bool()>   is_on)
    : QtButtonBase(world, std::move(callback)),
      Fl_Button(QtBase::x, QtBase::y, w, QtBase::h, label),
      on_color(on_color),
      text_color(text_color),
      is_on(std::move(is_on))
{
    box(FL_UP_BOX);
    color(FL_GRAY);
    align(FL_ALIGN_INSIDE | FL_ALIGN_CENTER);

    auto width = fl_width(label);
    if (width > w) {
        int         index = midpoint(label);
        std::string new_label(label, 0, index);
        new_label += "\n";
        new_label += label + index;
        QtBase::h *= 2;
        copy_label(new_label.c_str());
        Fl_Widget::resize(QtBase::x, QtBase::y, w, QtBase::h);
    }

    QtBase::y += QtBase::h + 10;
}

int QtToggleButton::handle(int event)
{
    if (event == FL_PUSH && QtButtonBase::callback) {
        return QtButtonBase::callback(event);  // Example argument
    }
    return 0;
}

void QtToggleButton::draw()
{
    debug_output("Drawing QtToggleButton... ");
    DEBUG_USE(auto start = std::chrono::steady_clock::now(););
    fl_color(FL_BLACK);
    fl_rectf(Fl_Button::x(), Fl_Button::y(), Fl_Button::w(), Fl_Button::h());

    if (is_on()) {
        fl_color(on_color);
    } else {
        fl_color(QtButtonBase::default_off_color);
    }
    fl_rectf(Fl_Button::x(), Fl_Button::y(), Fl_Button::w() - 2,
             Fl_Button::h() - 2);

    fl_color(text_color);
    fl_font(FL_HELVETICA_BOLD, 14);
    int  text_width = fl_width(label()) + 1;
    auto parts      = split(label(), '\n', 2);
    if (parts.size() > 1) {
        auto text_width1 = fl_width(parts[0].c_str()) + 1;
        auto text_width2 = fl_width(parts[1].c_str()) + 1;
        fl_draw(parts[0].c_str(),
                Fl_Button::x() + (Fl_Button::w() - text_width1) / 2,
                Fl_Button::y() + (Fl_Button::h() - fl_height()) / 2);
        fl_draw(parts[1].c_str(),
                Fl_Button::x() + (Fl_Button::w() - text_width2) / 2,
                Fl_Button::y() + (Fl_Button::h() + fl_height()) / 2 + 4);
    } else {
        fl_draw(label(), Fl_Button::x() + (Fl_Button::w() - text_width) / 2,
                Fl_Button::y() + (Fl_Button::h() + fl_height()) / 2 - 4);
    }

    DEBUG_USE(auto end = std::chrono::steady_clock::now(););
    DEBUG_USE(auto duration =
                  std::chrono::duration_cast<std::chrono::microseconds>(
                      end - start););
    debug_output("Time to draw QtToggleButton: ", duration.count(),
                 " microseconds\n");
}

ControlWindow::ControlWindow(World* world, int start_x, int W, int H)
    : Fl_Window(start_x, 0, W, H, "Control Window"), world(world)
{
    box(FL_UP_BOX);
    color(FL_LIGHT2);
    align(FL_ALIGN_INSIDE | FL_ALIGN_TOP | FL_ALIGN_LEFT);

    static int const button_width = W - 20;

    create_button(
        button_width, "Pause/Step", FL_BLACK, QtButtonBase::default_on_color,
        [world, this](int) {
            if (World::is_paused) {
                world->tick();
            } else {
                World::pause();
            }
            redraw();
            return 1;  // Indicate handled
        },
        [] { return World::is_paused; });

    create_button(button_width, "Resume Simulation", FL_BLACK, FL_GRAY,
                  [this](int) {
                      if (World::is_paused)
                          World::unpause();
                      redraw();
                      return 1;  // Indicate handled
                  });

    create_separator(button_width);

    create_button(button_width, "Clear Vehicle Selection", FL_BLACK, FL_GRAY,
                  [world](int) {
                      world->clear_verbose_vehicles();

                      return 1;  // Indicate handled
                  });

    create_button(
        button_width, "Toggle Sought Vehicles", FL_BLACK,
        QtButtonBase::default_on_color,
        [this](int) {
            World::view_mode.toggle(World::ViewMode::VEHICLE_SEEKING);
            redraw();
            return 1;  // Indicate handled
        },
        [] {
            return World::view_mode.is_set(World::ViewMode::VEHICLE_SEEKING);
        });

    create_button(
        button_width, "Toggle Sought Food", FL_BLACK,
        QtButtonBase::default_on_color,
        [this](int) {
            World::view_mode.toggle(World::ViewMode::FOOD_SEEKING);
            redraw();
            return 1;  // Indicate handled
        },
        [] { return World::view_mode.is_set(World::ViewMode::FOOD_SEEKING); });

    create_separator(button_width);

    create_button(button_width, "Add Vehicle", FL_BLACK, FL_GRAY, [world](int) {
        world->create_vehicle(world->rand_pos_in_bounds());
        return 1;  // Indicate handled
    });

    create_button(
        button_width, "Feed Mode", FL_BLACK, QtButtonBase::default_on_color,
        [this, world](int) {
            World::interact_mode.toggle(World::InteractMode::FEED);
            if (!World::interact_mode.is_set(World::InteractMode::FEED)) {
                redraw();
                return 1;  // Indicate handled
            }
            auto s = fl_input("Enter amount of food");
            if (s) {
                try {
                    world->feed_count = std::stoi(s);
                } catch (std::exception&) {
                    fl_alert("Invalid count. Using default 10.");
                    world->feed_count = 10;
                }
                World::interact_mode.unset(World::InteractMode::KILL);
            } else {
                World::interact_mode.unset(World::InteractMode::FEED);
            }
            redraw();
            return 1;  // Indicate handled
        },
        [] { return World::interact_mode.is_set(World::InteractMode::FEED); });

    create_button(button_width, "Add a Lot of Feed", FL_BLACK, FL_BLACK,
                  [world](int) {
                      auto s = fl_input("Enter amount of food to add");
                      if (s == nullptr) {
                          return 0;
                      }
                      int count = std::stol(s);
                      for (int i = 0; i < count; i++) {
                          world->new_random_food();
                      }
                      return 1;  // Indicate handled
                  });

    create_button(
        button_width, "Kill Mode", FL_BLACK,
        QtButtonBase::default_warning_color,
        [this](int) {
            World::interact_mode.toggle(World::InteractMode::KILL);
            if (!World::interact_mode.is_set(World::InteractMode::KILL)) {
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
                World::interact_mode.unset(World::InteractMode::FEED);
            } else {
                World::interact_mode.unset(World::InteractMode::KILL);
            }
            redraw();
            return 1;  // Indicate handled
        },
        [] { return World::interact_mode.is_set(World::InteractMode::KILL); });

    create_separator(button_width);
    // buttons.push_back(std::make_unique<QtSeparator>(button_width));

    create_button(
        button_width, "Change Food Spawn Chance", FL_BLACK, FL_BLACK,
        [world](int) {
            auto s = fl_input("Enter new food spawn chance (0-100)");
            if (s) {
                try {
                    auto pct = std::stod(s);
                    if (pct < 0.0 || pct > 100.0) {
                        throw std::out_of_range("Out of range");
                    }
                    world->food_pct_chance = pct;
                    return 1;
                } catch (std::exception&) {
                    fl_alert(
                        "Invalid input. Must be a number between 0 and 100");
                }
            }
            return 0;
        });

    create_button(
        button_width, "Change Max Food", FL_BLACK, FL_BLACK, [world](int) {
            auto s = fl_input("Enter new maximum food amount");
            if (s) {
                try {
                    auto count      = std::stoul(s);
                    world->max_food = count;
                    return 1;
                } catch (std::exception&) {
                    fl_alert("Invalid input. Must be a positive number");
                }
            }
            return 0;
        });

    create_separator(button_width);

    create_button(button_width, "End Simulation",
                  QtButtonBase::default_warning_color, FL_GRAY, [](int) {
                      World::stop_running(0);
                      render::FLTKRenderer::teardown();
                      return 1;  // Indicate handled
                  });
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

void ControlWindow::create_button(int                     button_width,
                                  char const*             label,
                                  Fl_Color                text_color,
                                  Fl_Color                on_color,
                                  std::function<int(int)> callback,
                                  std::function<bool()>   is_on)
{
    buttons.push_back(std::make_unique<QtToggleButton>(
        world, button_width, label, text_color, on_color, std::move(callback),
        std::move(is_on)));
}

void ControlWindow::create_separator(int button_width)
{
    buttons.push_back(std::make_unique<QtSeparator>(button_width));
}

}  // namespace tom::render