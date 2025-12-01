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
                               std::function<int(int)> callback)
    : QtButtonBase(world, std::move(callback)),
      Fl_Button(QtButtonBase::x, QtButtonBase::y, w, QtButtonBase::h, label)
{
    box(FL_UP_BOX);
    color(FL_GRAY);
    align(FL_ALIGN_INSIDE | FL_ALIGN_CENTER);

    QtButtonBase::y += QtButtonBase::h + 10;
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

QtInput::QtInput(World*                  world,
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

int QtInput::handle(int event)
{
    if (event == FL_PUSH) {
        if (QtButtonBase::callback) {
            return QtButtonBase::callback(event);  // Example argument
        }
        return 0;
    }
    return Fl_Input::handle(event);
}

ControlWindow::ControlWindow(World* world, int startX, int W, int H)
    : Fl_Window(startX, 0, W, H, "Control Window"), world(world)
{
    box(FL_UP_BOX);
    color(FL_LIGHT2);
    align(FL_ALIGN_INSIDE | FL_ALIGN_TOP | FL_ALIGN_LEFT);

    static int const buttonWidth = W - 20;

    buttons.push_back(std::make_unique<QtToggleButton>(
        world, buttonWidth, "Toggle Quadtree", [](int) {
            World::toggleQuadtree();
            return 1;  // Indicate handled
        }));

    buttons.push_back(std::make_unique<QtToggleButton>(
        world, buttonWidth, "Pause/Step", [world](int) {
            if (World::isPaused) {
                world->tick();
            } else {
                World::pause();
            }
            return 1;  // Indicate handled
        }));

    buttons.push_back(std::make_unique<QtToggleButton>(
        world, buttonWidth, "Resume Simulation", [](int) {
            if (World::isPaused)
                World::unpause();
            return 1;  // Indicate handled
        }));

    buttons.push_back(std::make_unique<QtToggleButton>(
        world, buttonWidth, "Clear Vehicle Selection", [world](int) {
            world->clearVerboseVehicles();
            return 1;  // Indicate handled
        }));

    buttons.push_back(std::make_unique<QtToggleButton>(
        world, buttonWidth, "Add Vehicle", [world](int) {
            world->createVehicle(world->randomPosition());
            return 1;  // Indicate handled
        }));

    buttons.push_back(std::make_unique<QtToggleButton>(
        world, buttonWidth, "Stop Simulation", [](int) {
            World::stopRunning(0);
            FLTKRenderer::teardown();
            return 1;  // Indicate handled
        }));

    buttons.push_back(std::make_unique<QtToggleButton>(
        world, buttonWidth, "Kill Mode", [](int) {
            World::killMode = !World::killMode;
            if (!World::killMode)
                return 1;  // Indicate handled
            auto s = fl_input("Enter kill radius");
            if (s) {
                try {
                    World::killRadius = std::stoi(s);
                } catch (std::exception&) {
                    fl_alert("Invalid radius input. Using default 100.");
                    World::killRadius = 100;
                }
            } else {
                World::killMode = false;
            }
            return 1;  // Indicate handled
        }));

    buttons.push_back(std::make_unique<QtToggleButton>(
        world, buttonWidth, "Toggle Sought Vehicles", [](int) {
            World::showSoughtVehicles = !World::showSoughtVehicles;
            return 1;  // Indicate handled
        }));

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