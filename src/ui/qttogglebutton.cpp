#include "ui/qttogglebutton.h"
#include "split.h"

namespace tom::render {

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
    auto parts      = split(label(), '\n', 2ul);
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
}  // namespace tom::render