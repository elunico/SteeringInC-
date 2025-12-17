#ifndef CONTROLS_H
#define CONTROLS_H

#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/fl_draw.H>

#include <FL/Fl_Window.H>
#include <functional>
#include <memory>
#include "qtbase.h"

#include "irenderer.h"

namespace tom::render {

struct QtSeparator : QtBase, Fl_Box {
    QtSeparator(int w);
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