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

    static Fl_Color default_on_color;
    static Fl_Color default_off_color;
    static Fl_Color default_warning_color;

    World*                  world;
    std::function<int(int)> callback;

    QtButtonBase(World* world, std::function<int(int)> callback)
        : world(world), callback(std::move(callback))
    {
    }

    virtual ~QtButtonBase();
};

struct QtToggleButton : public QtButtonBase, public Fl_Button {
    Fl_Color              on_color;
    Fl_Color              text_color;
    std::function<bool()> is_on;
    QtToggleButton(
        World*                  world,
        int                     w,
        char const*             label,
        Fl_Color                text_color,
        Fl_Color                on_color,
        std::function<int(int)> callback,
        std::function<bool()>   is_on = [] { return false; });
    int  handle(int event) override;
    void draw() override;
};

struct ControlWindow : public Fl_Window {
    World*                                     world;
    std::vector<std::unique_ptr<QtButtonBase>> buttons;

    ControlWindow(World* world, int start_x, int W, int H);
    int  handle(int event) override;
    void draw() override;
};

#endif