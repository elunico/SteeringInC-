#include "qtbuttonbase.h"
#include "qtbase.h"

namespace tom::render {

Fl_Color QtButtonBase::default_on_color = fl_rgb_color(0, 225, 80);

Fl_Color QtButtonBase::default_off_color = fl_rgb_color(200, 200, 200);

Fl_Color QtButtonBase::default_warning_color = fl_rgb_color(255, 30, 30);

QtButtonBase::QtButtonBase(World* world, std::function<int(int)> callback)
    : QtBase(QtButtonBase::button_height),
      world(world),
      callback(std::move(callback))
{
}

QtButtonBase::~QtButtonBase() = default;

}  // namespace tom::render