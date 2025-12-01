#ifndef CONTROLS_H
#define CONTROLS_H

#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/fl_draw.H>

#include <FL/Fl_Window.H>
#include <functional>
#include <memory>

#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Input.H>
#include "irenderer.h"

struct QtButtonBase {
    static int           x;
    static int           y;
    static constexpr int h = 30;

    World*                  world;
    std::function<int(int)> callback;

    QtButtonBase(World* world, std::function<int(int)> callback)
        : world(world), callback(std::move(callback))
    {
    }

    virtual ~QtButtonBase();
};

struct QtInput : QtButtonBase, Fl_Input {
    QtInput(World*                  world,
            int                     w,
            char const*             label,
            std::function<int(int)> callback);
    int handle(int event) override;
};

struct QtToggleButton : QtButtonBase, Fl_Button {
    QtToggleButton(World*                  world,
                   int                     w,
                   char const*             label,
                   std::function<int(int)> callback);
    int handle(int event) override;
};

struct QtCheckButton : QtButtonBase, Fl_Check_Button {
    QtCheckButton(World*                  world,
                  int                     w,
                  char const*             label,
                  std::function<int(int)> callback);
    int handle(int event) override;
};

struct ControlWindow : public Fl_Window {
    World*                                     world;
    std::vector<std::unique_ptr<QtButtonBase>> buttons;

    ControlWindow(World* world, int startX, int W, int H);
    int  handle(int event) override;
    void draw() override;
};

#endif