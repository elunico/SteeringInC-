#ifndef CONTROLS_H
#define CONTROLS_H

#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/fl_draw.H>

#include <FL/Fl_Window.H>
#include <functional>
#include <memory>
#include <type_traits>

#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Input.H>
#include "irenderer.h"

namespace tom::render {

struct QtBase {
    static int x;
    static int y;
    int        h = 30;

    QtBase(int h);
    virtual ~QtBase() = default;
};

struct QtButtonBase : QtBase {
    static Fl_Color      default_on_color;
    static Fl_Color      default_off_color;
    static Fl_Color      default_warning_color;
    static constexpr int button_height = 30;

    World*                  world;
    std::function<int(int)> callback;

    QtButtonBase(QtButtonBase const&)            = delete;
    QtButtonBase& operator=(QtButtonBase const&) = delete;

    QtButtonBase(World* world, std::function<int(int)> callback);

    virtual ~QtButtonBase();
};

struct QtSeparator : QtBase, Fl_Box {
    QtSeparator(int w);
    void draw() override;
};

struct QtToggleButton : public QtButtonBase, public Fl_Button {
    Fl_Color              on_color;
    Fl_Color              text_color;
    std::function<bool()> is_on;
    QtToggleButton(QtToggleButton const&)            = delete;
    QtToggleButton& operator=(QtToggleButton const&) = delete;
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
    World*                               world;
    std::vector<std::unique_ptr<QtBase>> buttons;

    ControlWindow()                                = delete;
    ControlWindow(ControlWindow const&)            = delete;
    ControlWindow& operator=(ControlWindow const&) = delete;

    ControlWindow(World* world, int start_x, int W, int H);
    int  handle(int event) override;
    void draw() override;

    void create_button(
        int                     button_width,
        char const*             label,
        Fl_Color                text_color,
        Fl_Color                on_color,
        std::function<int(int)> callback,
        std::function<bool()>   is_on = [] { return false; });

    void create_separator(int button_width);
};
}  // namespace tom::render
#endif