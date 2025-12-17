#ifndef QTTOGGLEBUTTON_H
#define QTTOGGLEBUTTON_H

#include "FL/Fl_Button.H"
#include "qtbuttonbase.h"

namespace tom::render {
struct QtToggleButton : QtButtonBase, Fl_Button {
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

}  // namespace tom::render

#endif  // QTTOGGLEBUTTON_H