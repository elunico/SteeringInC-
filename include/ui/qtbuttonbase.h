#ifndef QTBUTTONBASE_H
#define QTBUTTONBASE_H

#include <FL/fl_draw.H>
#include <functional>
#include "qtbase.h"
#include "world.h"

namespace tom::render {

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

}  // namespace tom::render

#endif  // QTBUTTONBASE_H